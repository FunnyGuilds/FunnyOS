#include <FunnyOS/Kernel/MM/PhysicalMemoryManager.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Stdlib/Math.hpp>
#include <FunnyOS/Kernel/Kernel.hpp>

#define PMM_PREFIX "PMM: "

namespace FunnyOS::Kernel::MM {
    namespace {

        /**
         * Represents a result of a PhysicalMemoryManager::AllocatePagesRaw(0)
         *
         * It is not a valid address but it is a valid arguments for all FreePages* functions.
         */
        constexpr const physicaladdress_t ZERO_PAGE_INDEX =
            Stdlib::NumeralTraits::Info<physicaladdress_t>::MaximumValue;

        /**
         * Splits the region in half at the address specified in [addres].
         *
         * Example:
         *  If region [0x1000 - 0x2000] is split at location [0x1500].
         *  [region] is being shortened to [0x1000 - 0x1500].
         *  And a new region at [0x1500 - 0x2000] is returned.
         *
         * @param[in, out] region - region that is being split, its higher half will be removed
         * @param address point at where to split the region
         * @return the higher half of the region
         */
        MemoryRegion SplitRegion(MemoryRegion& region, physicaladdress_t address) {
            FK_LOG_DEBUG_F(PMM_PREFIX "Split happening in region 0x%016llx -> 0x%016llx at 0x%016llx",
                           region.RegionStart, region.RegionEnd, address);

            F_ASSERT(IsInRegion(address, region), PMM_PREFIX "address not in region for split");

            MemoryRegion higherRegion = region;
            higherRegion.RegionStart = address;
            region.RegionEnd = address;
            return higherRegion;
        }

        /**
         * Gets a string representation of a memory map type.
         */
        const char* GetMemoryTypeString(Bootparams::MemoryMapEntryType type) {
            using Bootparams::MemoryMapEntryType;

            switch (type) {
                case MemoryMapEntryType::AvailableMemory:
                    return "Available";
                case MemoryMapEntryType::ReservedMemory:
                    return "Reserved";
                case MemoryMapEntryType::ACPIReclaimMemory:
                    return "ACPIReclaimable";
                case MemoryMapEntryType::ACPINVSMemory:
                    return "ACPINVS";
                case MemoryMapEntryType::Reserved:
                    return "Reserved";
                case MemoryMapEntryType::KernelImage:
                    return "KernelImage";
                case MemoryMapEntryType::PageTableReclaimable:
                    return "PageTableReclaimable";
                case MemoryMapEntryType::LongMemReclaimable:
                    return "LongMemReclaimable";
                default:
                    return "Unknown";
            }
        }

        /**
         * Dumps a memory map entry information to a kernel log output.
         */
        void DumpMemoryMapEntry(Bootparams::MemoryMapEntryType type, const char* acpi, physicaladdress_t start,
                                physicaladdress_t end) {
            FK_LOG_DEBUG_F(PMM_PREFIX "\t(0x%016llx -> 0x%016llx) L 0x%016llx T %20s A %03s", start, end, (end - start),
                           GetMemoryTypeString(type), acpi);
        }

        /**
         * Gets a location of a bit in the memory control block bitmap.
         *
         * @param block memory control block
         * @param bit bit to look for
         * @param offset offset of the bit in the returned byte
         * @return byte containing the bit
         */
        uint8_t* _FetchLocationInBitmap(MemoryChunkControlBlock& block, size_t bit, uint8_t& offset) {
            F_ASSERT(bit < block.AllocablePagesCount, PMM_PREFIX "bit > AllocablePagesCount");
            offset = bit % 8;
            return PhysicalAddressToPointer<uint8_t>(block.BitmapStart) + (bit / 8);
        }

        /**
         * Sets a bit in a memory control block bitmap.
         *
         * @param block memory control block in which the bit should be set in.
         * @param bit bit to be set
         * @param status [true] to set the bit to [1], [false] to set the bit to [0]
         */
        bool SetBitInBitmap(MemoryChunkControlBlock& block, size_t bit, bool status) {
            uint8_t offset;
            uint8_t* byte = _FetchLocationInBitmap(block, bit, offset);
            uint8_t oldValue = *byte;

            if (status) {
                *byte |= (1 << offset);
            } else {
                *byte &= ~(1 << offset);
            }

            return *byte != oldValue;
        }

        /**
         * Gets a bit in a memory control block bitmap.
         *
         * @param block memory control block from which the bit should be get from.
         * @param bit bit to be read
         * @return  [true] if the bit is set to [1], [false] if it is set to [0]
         */
        bool GetBitInBitmap(MemoryChunkControlBlock& block, size_t bit) {
            uint8_t offset;
            uint8_t* byte = _FetchLocationInBitmap(block, bit, offset);
            return (*byte & (1 << offset)) != 0;
        }
    }  // namespace

    bool IsMemoryTypeAvailable(Bootparams::MemoryMapEntryType type) {
        return type == Bootparams::MemoryMapEntryType::AvailableMemory;
    }

    bool IsMemoryTypeReclaimable(Bootparams::MemoryMapEntryType type) {
        return type == Bootparams::MemoryMapEntryType::PageTableReclaimable ||
               type == Bootparams::MemoryMapEntryType::ACPIReclaimMemory ||
               type == Bootparams::MemoryMapEntryType::LongMemReclaimable;
    }

    bool IsMemoryMapEntryUsable(const Bootparams::MemoryMapDescription& map, size_t entryIndex) {
        using namespace Bootparams;

        const auto& entry = map.First[entryIndex];

        if (map.HasAcpiExtendedAttribute) {
            if ((entry.ACPIFlags & ACPI30Flags::DONT_IGNORE) != ACPI30Flags::DONT_IGNORE) {
                return false;
            }
            if ((entry.ACPIFlags & ACPI30Flags::MEMORY_VOLATILE) != 0) {
                return false;
            }
        }

        return IsMemoryTypeAvailable(entry.Type) || IsMemoryTypeReclaimable(entry.Type);
    }

    bool IsInRegion(physicaladdress_t address, const MemoryRegion& region) {
        return address >= region.RegionStart && address < region.RegionEnd;
    }

    unsigned int GetMemoryTypeWeight(Bootparams::MemoryMapEntryType type) {
        if (IsMemoryTypeAvailable(type)) {
            return 0;
        }
        if (IsMemoryTypeReclaimable(type)) {
            return 1;
        }

        return Stdlib::NumeralTraits::Info<unsigned int>::MaximumValue;
    }

    MemoryChunkControlBlock& MemoryRegion::GetControlBlock() {
        return *PhysicalAddressToPointer<MemoryChunkControlBlock>(RegionStart);
    }

    uint64_t MemoryRegion::GetLength() const {
        return RegionEnd - RegionStart;
    }

    uint64_t MemoryStatistics::GetTotalWastedMemory() const {
        return UnusableUnalignedMemory + UnusableFragmentedMemory + UnusableLowMemory;
    }

    uint64_t MemoryStatistics::GetTotalUnusableMemory() const {
        return ControlBlockWaste + KernelImageSize + GetTotalWastedMemory();
    }

    void PhysicalMemoryManager::Initialize(const Bootparams::MemoryMapDescription& map) {
        FK_LOG_INFO(PMM_PREFIX "Initializing...");

        // Debug logs
        FK_LOG_DEBUG(PMM_PREFIX "Initial E820 memory map received from the bootloader");
        for (size_t i = 0; i < map.Count; i++) {
            const auto& entry = map.First[i];

            char acpi[8] = "N/A";
            if (map.HasAcpiExtendedAttribute) {
                Stdlib::String::StringBuffer acpiBuffer{acpi, 8};
                Stdlib::String::IntegerToString(acpiBuffer, entry.ACPIFlags, 2);
            }

            DumpMemoryMapEntry(entry.Type, acpi, entry.BaseAddress, entry.BaseAddress + entry.Length);
        }

        // Prepare region map
        m_memoryRegions.Clear();
        m_memoryMap.Clear();
        m_memoryRegions.EnsureCapacity(map.Count);
        m_memoryMap.EnsureCapacity(map.Count);

        // Reset memory statistics
        m_memoryStatistics.ControlBlockWaste = 0;
        m_memoryStatistics.UnusableUnalignedMemory = 0;
        m_memoryStatistics.UnusableLowMemory = 0x100000;
        m_memoryStatistics.KernelImageSize = 0;
        m_memoryStatistics.TotalReclaimableMemory = 0;
        m_memoryStatistics.TotalAvailableMemory = 0;

        // Turn map memory entries into regions, save memory map and find memory top
        m_physicalMemoryTop = 0;

        for (size_t i = 0; i < map.Count; i++) {
            const auto& memoryMapEntry = map.First[i];

            auto& region = m_memoryRegions.AppendInPlace();
            region.RegionMemoryType = memoryMapEntry.Type;
            region.RegionStart = memoryMapEntry.BaseAddress;
            region.RegionEnd = memoryMapEntry.BaseAddress + memoryMapEntry.Length;
            region.IsUsable = IsMemoryMapEntryUsable(map, i);
            region.IsInitialized = false;
            region.IsReady = false;

            if (region.RegionMemoryType == Bootparams::MemoryMapEntryType::KernelImage) {
                m_memoryStatistics.KernelImageSize += region.GetLength();
            } else if (IsMemoryTypeReclaimable(region.RegionMemoryType)) {
                m_memoryStatistics.TotalReclaimableMemory += region.GetLength();
            }

            m_memoryMap.Append(memoryMapEntry);
            m_physicalMemoryTop = Stdlib::Max(m_physicalMemoryTop, region.RegionEnd);
        }

        // Check for overlapping regions
        RemoveOverlappingRegions();

        // Remove unusable entries
        ClearUnusableRegions();

        // Initially only the first 4GB of physical memory is mapped so we cannot now initialize blocks that span
        // beyond the 4 GB boundary, instead any block that crosses that boundary is split into half, the lower part
        // below this boundary is marked as available and the higher part as LongMemReclaimable.
        constexpr const physicaladdress_t BOUNDARY_4GB = 1024ULL * 1024ULL * 1024ULL * 4ULL;

        for (auto& region : m_memoryRegions) {
            if (region.RegionMemoryType != Bootparams::MemoryMapEntryType::AvailableMemory) {
                continue;
            }

            if (region.RegionStart >= BOUNDARY_4GB) {
                region.RegionMemoryType = Bootparams::MemoryMapEntryType::LongMemReclaimable;
                continue;
            }

            if (!IsInRegion(BOUNDARY_4GB, region)) {
                continue;
            }

            MemoryRegion highRegion = SplitRegion(region, BOUNDARY_4GB);
            highRegion.RegionMemoryType = Bootparams::MemoryMapEntryType::LongMemReclaimable;
            m_memoryRegions.Append(Stdlib::Move(highRegion));
        }

        // Set all AvailableMemory regions as ready
        for (auto& region : m_memoryRegions) {
            if (region.RegionMemoryType != Bootparams::MemoryMapEntryType::AvailableMemory) {
                continue;
            }

            region.IsReady = true;
        }

        InitializeMemoryRegions();
        m_memoryRegions.ShrinkToSize();
        FK_LOG_OK(PMM_PREFIX "Initialized!");
    }

    void PhysicalMemoryManager::ReclaimMemory(Bootparams::MemoryMapEntryType type) {
        FK_LOG_DEBUG_F(PMM_PREFIX "Trying to reclaim memory of type %s", GetMemoryTypeString(type));
        F_ASSERT(IsMemoryTypeReclaimable(type), PMM_PREFIX "given memory type is not reclaimable");

        bool anyReclaims = false;

        for (auto& region : m_memoryRegions) {
            F_ASSERT(region.IsUsable, PMM_PREFIX "unusable memory region on reclaim stage");

            if (region.RegionMemoryType != type) {
                continue;
            }

            region.RegionMemoryType = Bootparams::MemoryMapEntryType::AvailableMemory;
            region.IsReady = true;
            anyReclaims = true;

            m_memoryStatistics.TotalReclaimableMemory -= region.GetLength();
        }

        if (anyReclaims) {
            InitializeMemoryRegions();
        } else {
            FK_LOG_DEBUG_F(PMM_PREFIX "Nothing to reclaim for type %s", GetMemoryTypeString(type));
        }
    }

    physicaladdress_t PhysicalMemoryManager::AllocatePagesRaw(size_t pages) {
        if (pages == 0) {
            return ZERO_PAGE_INDEX;
        }

        for (auto& region : m_memoryRegions) {
            if (!region.IsInitialized) {
                continue;
            }

            auto& block = region.GetControlBlock();
            if (block.FreePages < pages) {
                continue;
            }

            // TODO: Faster free memory search algorithm

            // Search for a [pages] of subsequent free pages
            size_t subsequentFreePages = 0;
            size_t validOffset = 0;

            for (size_t i = 0; i < block.AllocablePagesCount; i++) {
                if (!GetBitInBitmap(block, i)) {
                    subsequentFreePages++;
                } else {
                    subsequentFreePages = 0;
                }

                if (subsequentFreePages == pages) {
                    // Set [validOffset] to the first page of this subsequent chunk
                    validOffset = i - subsequentFreePages + 1;
                    break;
                }
            }

            if (subsequentFreePages < pages) {
                // Nothing found, continue search in another block
                continue;
            }

            for (size_t i = 0; i < pages; i++) {
                // Mark entries as allocated
                SetBitInBitmap(block, validOffset + i, true);
            }

            // Update free pages
            block.FreePages -= pages;

            // Return base
            return block.FirstPageBegin + (validOffset * PAGE_SIZE);
        }

        return NULL_ADDRESS;
    }

    void PhysicalMemoryManager::FreePagesRaw(physicaladdress_t base, size_t pages) {
        if (base == ZERO_PAGE_INDEX || pages == 0) {
            return;
        }

        const uintptr_t address = reinterpret_cast<uintptr_t>(base);
        if ((address % PAGE_SIZE) != 0) {
            FK_LOG_WARNING_F(PMM_PREFIX "Trying to free page at address 0x%016llx, but the address is not page aligned",
                             address);
            return;
        }

        // Find entry
        auto entry = FindEntryForRegion(address);

        if (!entry) {
            FK_LOG_WARNING_F(PMM_PREFIX "Trying to free page at address 0x%016llx, but it is outside available memory",
                             address);
            return;
        }

        if (address + pages * PAGE_SIZE >= entry->RegionEnd) {
            FK_LOG_WARNING_F(
                PMM_PREFIX
                "Trying to free %zu pages at address 0x%016llx, but the subsequent pages are in different memory "
                "chunks",
                pages, address);
            return;
        }

        if (!entry->IsInitialized) {
            FK_LOG_WARNING_F(PMM_PREFIX
                             "Trying to free page at address 0x%016llx, but it is in not-initialized memory chunk",
                             address);
            return;
        }

        auto& controlBlock = entry->GetControlBlock();
        const size_t pageOffset = (base - controlBlock.FirstPageBegin) / PAGE_SIZE;
        if (pageOffset < controlBlock.ControlBlockPageSpan) {
            FK_LOG_WARNING_F(PMM_PREFIX
                             "Trying to free page at address 0x%016llx, but that page belongs to a control block",
                             address);
            return;
        }

        for (size_t i = 0; i < pages; i++) {
            if (!SetBitInBitmap(controlBlock, pageOffset + i, false)) {
                FK_LOG_WARNING_F(
                    PMM_PREFIX
                    "Double free detected while freeing %zu pages at 0x%016llx, page at index %zu is already "
                    "freed",
                    pages, address, i);
                return;
            }
        }
    }

    physicaladdress_t PhysicalMemoryManager::AllocatePage() {
        return AllocatePagesRaw(1);
    }

    void PhysicalMemoryManager::FreePage(physicaladdress_t page) {
        FreePagesRaw(page, 1);
    }

    PageBuffer PhysicalMemoryManager::AllocatePages(size_t pages) {
        PageBuffer buffer;
        buffer.Start = AllocatePagesRaw(pages);
        buffer.Count = buffer.Start == NULL_ADDRESS ? 0 : pages;
        return buffer;
    }

    void PhysicalMemoryManager::FreePages(PageBuffer& buffer) {
        FreePagesRaw(buffer.Start, buffer.Count);
        buffer.Start = NULL_ADDRESS;
        buffer.Count = 0;
    }

    physicaladdress_t PhysicalMemoryManager::GetPhysicalMemoryTop() const {
        return m_physicalMemoryTop;
    }

    const Stdlib::Vector<Bootparams::MemoryMapEntry>& PhysicalMemoryManager::GetOriginalMemoryMap() const {
        return m_memoryMap;
    }

    PhysicalMemoryManager::PhysicalMemoryManager() = default;

    Stdlib::Optional<MemoryRegion> PhysicalMemoryManager::FindEntryForRegion(physicaladdress_t address) {
        for (auto& current : m_memoryRegions) {
            if (address < current.RegionStart) {
                continue;
            }

            if (address > current.RegionEnd) {
                continue;
            }

            return Stdlib::MakeOptional<MemoryRegion>(current);
        }

        return Stdlib::EmptyOptional<MemoryRegion>();
    }

    void PhysicalMemoryManager::InitializeMemoryRegions() {
        FK_LOG_DEBUG(PMM_PREFIX "InitializeMemoryRegions: dumping current parsed memory regions");
        DumpRegionsToDebug();

        for (auto& current : m_memoryRegions) {
            if (current.IsInitialized) {
                continue;
            }

            if (!current.IsUsable || !current.IsReady) {
                continue;
            }

            if (current.GetLength() < PAGE_SIZE * 2) {
                m_memoryStatistics.UnusableFragmentedMemory += current.GetLength();
                current.IsUsable = false;
                continue;
            }

            // First usable page base.
            const physicaladdress_t firstPage = AlignToPage(current.RegionStart);

            // How many bytes before the base of first usable page the control block is stored at.
            const physicaladdress_t controlBlockLocationRelative = current.RegionStart - firstPage;

            // How many allocable pages there are in the block.
            const size_t allocablePages = (current.GetLength() - controlBlockLocationRelative) / PAGE_SIZE;

            // Size of the bitmap (in bytes)
            const size_t bitmapSize = Stdlib::Math::DivideRoundUp(allocablePages, 8UL);

            // Total size of the control block with bitmap
            const size_t controlBlockSize = sizeof(MemoryChunkControlBlock) + bitmapSize;

            // How many usable pages the control block span
            const size_t controlBlockUsablePageSpan = Stdlib::Math::DivideRoundUp(
                static_cast<size_t>(controlBlockSize - controlBlockLocationRelative), PAGE_SIZE);

            auto& controlBlock = current.GetControlBlock();
            controlBlock.BitmapStart = current.RegionStart + sizeof(MemoryChunkControlBlock);
            controlBlock.AllocablePagesCount = allocablePages;
            controlBlock.FreePages = allocablePages;
            controlBlock.FirstPageBegin = firstPage;
            controlBlock.ControlBlockPageSpan = controlBlockUsablePageSpan;

            // Clear the bitmap
            Stdlib::Memory::SizedBuffer<uint8_t> bitmapBuffer{
                PhysicalAddressToPointer<uint8_t>(controlBlock.BitmapStart), bitmapSize};
            Stdlib::Memory::Set<uint8_t>(bitmapBuffer, 0);

            // Set control block as allocated in the bitmap
            for (size_t i = 0; i < controlBlockUsablePageSpan; i++) {
                SetBitInBitmap(controlBlock, i, true);
                controlBlock.FreePages--;
            }

            // Update statistics
            m_memoryStatistics.ControlBlockWaste +=
                controlBlockUsablePageSpan * PAGE_SIZE + controlBlockLocationRelative;

            m_memoryStatistics.UnusableUnalignedMemory += current.RegionEnd % PAGE_SIZE;
            m_memoryStatistics.TotalAvailableMemory += (allocablePages - controlBlockUsablePageSpan) * PAGE_SIZE;

            // Set region as initialized
            current.IsInitialized = true;
        }

        ClearUnusableRegions();
    }

    void PhysicalMemoryManager::RemoveOverlappingRegions() {
        bool hadChanges;

        do {
            hadChanges = false;

            for (auto& region : m_memoryRegions) {
                if (!region.IsUsable) {
                    // We do not care if two non-usable regions overlap.
                    continue;
                }

                // Gets the current region weight
                const unsigned int regionWeight = GetMemoryTypeWeight(region.RegionMemoryType);

                for (auto& other : m_memoryRegions) {
                    if (&other == &region) {
                        continue;
                    }

                    // Skip over invalidated regions
                    if (other.RegionStart == 0 && other.RegionEnd == 0) {
                        continue;
                    }

                    //
                    // Regions with higher weights overwrite regions with lower weights.
                    //
                    const unsigned int otherWeight = GetMemoryTypeWeight(other.RegionMemoryType);
                    if (regionWeight > otherWeight) {
                        continue;
                    }

                    // Check if the region's [region] start and end positions are contained in [other]
                    bool startInOther = IsInRegion(region.RegionStart, other);
                    bool endInOther = IsInRegion(region.RegionEnd - 1, other);

                    if (startInOther && endInOther) {
                        // [region] start and end positions are both in [other], it means that [region] is fully in
                        // [other], the entire [region] is invalidated in this case
                        region.RegionStart = 0;
                        region.RegionEnd = 0;
                        region.IsUsable = false;
                        hadChanges = true;

                        // [region] is no longed valid, no need to test it against anything anymore.
                        goto next_region;
                    }

                    if (startInOther) {
                        // Only the start of [region] is in [other].
                        // [region] must be split and the lower part (residing in [other]) is discarded.
                        // Only the higher half is kept in the list

                        // 0x1000 - 0x2000

                        MemoryRegion highRegion = SplitRegion(region, other.RegionEnd);
                        // [region] is discarded and replaced by [highRegion]
                        region = highRegion;
                        hadChanges = true;
                        continue;
                    }

                    if (endInOther) {
                        // Only the end of [region] is in [other].
                        // [region] must be split and the higher part (residing in [other]) is discarded.
                        // Only the lower half is kept in the list

                        static_cast<void>(SplitRegion(region, other.RegionStart));
                        // result is discarded
                        hadChanges = true;
                        continue;
                    }

                    if (IsInRegion(other.RegionStart, region) && IsInRegion(other.RegionEnd - 1, region)) {
                        // [other] resides fully in [region].
                        // [region] must be split in three and the middle part, overlapping [other] must be discarded.
                        MemoryRegion highRegion = SplitRegion(region, other.RegionStart);

                        // [region] is now the part before [other.RegionStart]
                        // [highRegion] is the part after [other.RegionStart], its lower half must be discarded
                        MemoryRegion newHighRegion = SplitRegion(highRegion, other.RegionEnd);
                        m_memoryRegions.Append(Stdlib::Move(newHighRegion));
                        // [highRegion] is discarded
                        hadChanges = true;
                        continue;
                    }
                }

            next_region:
                continue;
            }
        } while (hadChanges);

        // Remove previously invalidated memory regions
        Stdlib::RemoveIf(m_memoryRegions,
                         [](const MemoryRegion& region) { return region.RegionStart == 0 && region.RegionEnd == 0; });
    }

    void PhysicalMemoryManager::ClearUnusableRegions() {
        Stdlib::RemoveIf(m_memoryRegions, [](const MemoryRegion& region) { return !region.IsUsable; });
    }

    void PhysicalMemoryManager::DumpRegionsToDebug() {
        for (auto& current : m_memoryRegions) {
            DumpMemoryMapEntry(current.RegionMemoryType, "N/A", current.RegionStart, current.RegionEnd);
        }
    }

}  // namespace FunnyOS::Kernel::MM
