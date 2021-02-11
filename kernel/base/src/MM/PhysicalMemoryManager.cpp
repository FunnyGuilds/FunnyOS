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
         * Gets a string representation of a memory map type.
         */
        const char* GetMemoryTypeString(Bootparams::MemoryRegionType type) {
            using Bootparams::MemoryRegionType;

            switch (type) {
                case MemoryRegionType::AvailableMemory:
                    return "Available";
                case MemoryRegionType::ReservedMemory:
                    return "Reserved";
                case MemoryRegionType::ACPIReclaimMemory:
                    return "ACPIReclaimable";
                case MemoryRegionType::ACPINVSMemory:
                    return "ACPINVS";
                case MemoryRegionType::Reserved:
                    return "Reserved";
                case MemoryRegionType::KernelImage:
                    return "KernelImage";
                case MemoryRegionType::PageTableReclaimable:
                    return "PageTableReclaimable";
                case MemoryRegionType::LongMemReclaimable:
                    return "LongMemReclaimable";
                default:
                    return "Unknown";
            }
        }

        /**
         * Dumps a memory map entry information to a kernel log output.
         */
        void DumpMemoryMapEntry(Bootparams::MemoryRegionType type, physicaladdress_t start, physicaladdress_t end) {
            FK_LOG_DEBUG_F(
                PMM_PREFIX "\t(0x%016llx -> 0x%016llx) L 0x%016llx T %20s", start, end, (end - start),
                GetMemoryTypeString(type));
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

            const uint8_t oldValue = *byte;

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

    bool IsMemoryTypeAvailable(Bootparams::MemoryRegionType type) {
        return type == Bootparams::MemoryRegionType::AvailableMemory;
    }

    bool IsMemoryTypeReclaimable(Bootparams::MemoryRegionType type) {
        return type == Bootparams::MemoryRegionType::PageTableReclaimable ||
               type == Bootparams::MemoryRegionType::ACPIReclaimMemory ||
               type == Bootparams::MemoryRegionType::LongMemReclaimable;
    }

    bool IsMemoryMapEntryUsable(Bootparams::MemoryRegionType type) {
        return IsMemoryTypeAvailable(type) || IsMemoryTypeReclaimable(type);
    }

    MemoryChunkControlBlock& InitializedMemoryRegion::GetControlBlock() {
        return *PhysicalAddressToPointer<MemoryChunkControlBlock>(MemoryRegion.RegionStart);
    }

    uint64_t InitializedMemoryRegion::GetLength() const {
        return MemoryRegion.RegionEnd - MemoryRegion.RegionStart;
    }

    uint64_t MemoryStatistics::GetTotalWastedMemory() const {
        return UnusableUnalignedMemory + UnusableFragmentedMemory + UnusableLowMemory;
    }

    uint64_t MemoryStatistics::GetTotalUnusableMemory() const {
        return ControlBlockWaste + KernelImageSize + GetTotalWastedMemory();
    }

    void PhysicalMemoryManager::Initialize(const Stdlib::Vector<Bootparams::MemoryRegion>& map) {
        FK_LOG_INFO(PMM_PREFIX "Initializing...");

        // Debug logs
        FK_LOG_DEBUG(PMM_PREFIX "Initial E820 memory map received from the bootloader");
        for (const auto& entry : map) {
            DumpMemoryMapEntry(entry.Type, entry.RegionStart, entry.RegionEnd);
        }

        // Prepare region map
        m_memoryRegions.Clear();
        m_memoryRegions.EnsureCapacity(map.Size());

        // Reset memory statistics
        m_memoryStatistics.ControlBlockWaste        = 0;
        m_memoryStatistics.UnusableUnalignedMemory  = 0;
        m_memoryStatistics.UnusableLowMemory        = 0x100000;
        m_memoryStatistics.UnusableFragmentedMemory = 0;
        m_memoryStatistics.KernelImageSize          = 0;
        m_memoryStatistics.TotalReclaimableMemory   = 0;
        m_memoryStatistics.TotalAvailableMemory     = 0;

        // Turn map memory entries into regions, save memory map and find memory top
        m_physicalMemoryTop = 0;

        for (const auto& memoryMapEntry : map) {
            auto& region         = m_memoryRegions.AppendInPlace();
            region.MemoryRegion  = memoryMapEntry;
            region.IsInitialized = false;
            region.IsReady       = false;

            if (region.MemoryRegion.Type == Bootparams::MemoryRegionType::KernelImage) {
                m_memoryStatistics.KernelImageSize += region.GetLength();
            } else if (IsMemoryTypeReclaimable(region.MemoryRegion.Type)) {
                m_memoryStatistics.TotalReclaimableMemory += region.GetLength();
            }

            m_physicalMemoryTop = Stdlib::Max(m_physicalMemoryTop, region.MemoryRegion.RegionEnd);
        }

        // Remove unusable entries
        ClearUnusableRegions();

        // Set all AvailableMemory regions as ready
        for (auto& region : m_memoryRegions) {
            if (region.MemoryRegion.Type != Bootparams::MemoryRegionType::AvailableMemory) {
                continue;
            }

            region.IsReady = true;
        }

        InitializeMemoryRegions();
        m_memoryRegions.ShrinkToSize();
        FK_LOG_OK(PMM_PREFIX "Initialized!");
    }

    void PhysicalMemoryManager::ReclaimMemory(Bootparams::MemoryRegionType type) {
        FK_LOG_DEBUG_F(PMM_PREFIX "Trying to reclaim memory of type %s", GetMemoryTypeString(type));
        F_ASSERT(IsMemoryTypeReclaimable(type), PMM_PREFIX "given memory type is not reclaimable");

        bool anyReclaims = false;

        for (auto& region : m_memoryRegions) {
            F_ASSERT(region.IsUsable, PMM_PREFIX "unusable memory region on reclaim stage");

            if (region.MemoryRegion.Type != type) {
                continue;
            }

            region.MemoryRegion.Type = Bootparams::MemoryRegionType::AvailableMemory;
            region.IsReady           = true;

            anyReclaims = true;

            m_memoryStatistics.TotalReclaimableMemory -= region.GetLength();
        }

        if (anyReclaims) {
            InitializeMemoryRegions();
        } else {
            FK_LOG_DEBUG_F(PMM_PREFIX "Nothing to reclaim for type %s", GetMemoryTypeString(type));
        }
    }

    const Stdlib::Vector<InitializedMemoryRegion>& PhysicalMemoryManager::GetMemoryRegions() const {
        return m_memoryRegions;
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
            size_t validOffset         = 0;

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
            FK_LOG_WARNING_F(
                PMM_PREFIX "Trying to free page at address 0x%016llx, but the address is not page aligned", address);
            return;
        }

        // Find entry
        auto entry = FindEntryForRegion(address);

        if (!entry) {
            FK_LOG_WARNING_F(
                PMM_PREFIX "Trying to free page at address 0x%016llx, but it is outside available memory", address);
            return;
        }

        if (address + pages * PAGE_SIZE >= entry->MemoryRegion.RegionEnd) {
            FK_LOG_WARNING_F(
                PMM_PREFIX
                "Trying to free %zu pages at address 0x%016llx, but the subsequent pages are in different memory "
                "chunks",
                pages, address);
            return;
        }

        if (!entry->IsInitialized) {
            FK_LOG_WARNING_F(
                PMM_PREFIX "Trying to free page at address 0x%016llx, but it is in not-initialized memory chunk",
                address);
            return;
        }

        auto& controlBlock = entry->GetControlBlock();

        const size_t pageOffset = (base - controlBlock.FirstPageBegin) / PAGE_SIZE;
        if (pageOffset < controlBlock.ControlBlockPageSpan) {
            FK_LOG_WARNING_F(
                PMM_PREFIX "Trying to free page at address 0x%016llx, but that page belongs to a control block",
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

    PhysicalMemoryManager::PhysicalMemoryManager() = default;

    Stdlib::Optional<InitializedMemoryRegion> PhysicalMemoryManager::FindEntryForRegion(physicaladdress_t address) {
        for (auto& current : m_memoryRegions) {
            if (address < current.MemoryRegion.RegionStart) {
                continue;
            }

            if (address > current.MemoryRegion.RegionEnd) {
                continue;
            }

            return Stdlib::MakeOptional<InitializedMemoryRegion>(current);
        }

        return Stdlib::EmptyOptional<InitializedMemoryRegion>();
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
            const physicaladdress_t firstPage = AlignToPage(current.MemoryRegion.RegionStart);

            // How many bytes before the base of first usable page the control block is stored at.
            const physicaladdress_t controlBlockLocationRelative = current.MemoryRegion.RegionStart - firstPage;

            // How many allocable pages there are in the block.
            const size_t allocablePages = (current.GetLength() - controlBlockLocationRelative) / PAGE_SIZE;

            // Size of the bitmap (in bytes)
            const size_t bitmapSize = Stdlib::Math::DivideRoundUp(allocablePages, 8UL);

            // Total size of the control block with bitmap
            const size_t controlBlockSize = sizeof(MemoryChunkControlBlock) + bitmapSize;

            // How many usable pages the control block span
            const size_t controlBlockUsablePageSpan = Stdlib::Math::DivideRoundUp(
                static_cast<size_t>(controlBlockSize - controlBlockLocationRelative), PAGE_SIZE);

            auto& controlBlock                = current.GetControlBlock();
            controlBlock.BitmapStart          = current.MemoryRegion.RegionStart + sizeof(MemoryChunkControlBlock);
            controlBlock.AllocablePagesCount  = allocablePages;
            controlBlock.FreePages            = allocablePages;
            controlBlock.FirstPageBegin       = firstPage;
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

            m_memoryStatistics.UnusableUnalignedMemory += current.MemoryRegion.RegionEnd % PAGE_SIZE;
            m_memoryStatistics.TotalAvailableMemory += (allocablePages - controlBlockUsablePageSpan) * PAGE_SIZE;

            // Set region as initialized
            current.IsInitialized = true;
        }

        ClearUnusableRegions();
    }

    void PhysicalMemoryManager::ClearUnusableRegions() {
        Stdlib::RemoveIf(m_memoryRegions, [](const InitializedMemoryRegion& region) { return !region.IsUsable; });
    }

    void PhysicalMemoryManager::DumpRegionsToDebug() {
        for (auto& current : m_memoryRegions) {
            DumpMemoryMapEntry(
                current.MemoryRegion.Type, current.MemoryRegion.RegionStart, current.MemoryRegion.RegionEnd);
        }
    }


}  // namespace FunnyOS::Kernel::MM
