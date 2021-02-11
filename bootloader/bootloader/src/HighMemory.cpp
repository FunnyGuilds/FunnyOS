#include "HighMemory.hpp"

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Hardware/BIOS.hpp>
#include "Bootloader.hpp"

namespace FunnyOS::Bootloader64 {

    Stdlib::Vector<Bootparams::MemoryRegion>& GetMemoryMap() {
        static Stdlib::Vector<Bootparams::MemoryRegion> m_memoryMap;

        return m_memoryMap;
    }

    namespace {
        uint32_t GetRegionWeight(Bootparams::MemoryRegion& region) {
            switch (region.Type) {
                case Bootparams::MemoryRegionType::AvailableMemory:
                    return 1;
                case Bootparams::MemoryRegionType::ACPIReclaimMemory:
                case Bootparams::MemoryRegionType::PageTableReclaimable:
                case Bootparams::MemoryRegionType::LongMemReclaimable:
                    return 2;
                default:
                    return Stdlib::NumeralTraits::Info<unsigned int>::MaximumValue;
            }
        }

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
        Bootparams::MemoryRegion SplitRegion(Bootparams::MemoryRegion& region, Bootparams::physicaladdress_t address) {
            F_ASSERT(region.IsInRegion(address), "address not in region for split");

            Bootparams::MemoryRegion higherRegion = region;

            higherRegion.RegionStart = address;
            region.RegionEnd         = address;

            return higherRegion;
        }

        void RemoveInvalidRegions() {
            Stdlib::RemoveIf(GetMemoryMap(), [](const Bootparams::MemoryRegion& region) {
                return region.RegionStart == 0 && region.RegionEnd == 0;
            });
        }

    }  // namespace

    void RemoveOverlappingRegions() {
        bool hadChanges;

        do {
            hadChanges = false;

            for (auto& region : GetMemoryMap()) {
                if (region.RegionStart == 0 && region.RegionEnd == 0) {
                    // We do not care if two non-usable regions overlap.
                    continue;
                }

                // Gets the current region weight
                const unsigned int regionWeight = GetRegionWeight(region);

                for (auto& other : GetMemoryMap()) {
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
                    const unsigned int otherWeight = GetRegionWeight(other);
                    if (regionWeight > otherWeight) {
                        continue;
                    }

                    // Check if the region's [region] start and end positions are contained in [other]
                    const bool startInOther = other.IsInRegion(region.RegionStart);
                    const bool endInOther   = other.IsInRegion(region.RegionEnd - 1);

                    if (startInOther && endInOther) {
                        // [region] start and end positions are both in [other], it means that [region] is fully in
                        // [other], the entire [region] is invalidated in this case
                        region.RegionStart = 0;
                        region.RegionEnd   = 0;
                        hadChanges         = true;

                        // [region] is no longed valid, no need to test it against anything anymore.
                        goto next_region;
                    }

                    if (startInOther) {
                        // Only the start of [region] is in [other].
                        // [region] must be split and the lower part (residing in [other]) is discarded.
                        // Only the higher half is kept in the list

                        // 0x1000 - 0x2000

                        Bootparams::MemoryRegion highRegion = SplitRegion(region, other.RegionEnd);
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

                    if (region.IsInRegion(other.RegionStart) && region.IsInRegion(other.RegionEnd - 1)) {
                        // [other] resides fully in [region].
                        // [region] must be split in three and the middle part, overlapping [other] must be discarded.
                        Bootparams::MemoryRegion highRegion = SplitRegion(region, other.RegionStart);

                        // [region] is now the part before [other.RegionStart]
                        // [highRegion] is the part after [other.RegionStart], its lower half must be discarded
                        Bootparams::MemoryRegion newHighRegion = SplitRegion(highRegion, other.RegionEnd);
                        GetMemoryMap().Append(Stdlib::Move(newHighRegion));
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
        RemoveInvalidRegions();
    }

    void MergeAdjacentMemoryRegions() {
    next_region:
        for (auto& region : GetMemoryMap()) {
            if (region.RegionEnd == 0) {
                continue;
            }

            for (auto& otherRegion : GetMemoryMap()) {
                if (otherRegion.RegionEnd == 0) {
                    continue;
                }

                if (&region == &otherRegion) {
                    continue;
                }

                if (region.Type != otherRegion.Type || region.RegionEnd != otherRegion.RegionStart) {
                    continue;
                }

                region.RegionEnd = otherRegion.RegionEnd;

                otherRegion.RegionStart = 0;
                otherRegion.RegionEnd   = 0;

                goto next_region;
            }
        }

        RemoveInvalidRegions();
    }

    namespace {
        struct E820Response {
            uint64_t BaseAddress;
            uint64_t Length;
            uint32_t E820Type;
            uint32_t Ignored;
        } F_DONT_ALIGN;

        void CallE820(E820Response& response, uint32_t& continuation) {
            uint32_t eaxValue;
            uint32_t actualSize;
            uint32_t error;

            const bool callOk = HW::BIOS::CallBios(
                0x15, "eax, edx, ebx, ecx, es:di, =eax, =ebx, =ecx, =ah", 0x0000E820, 0x534D4150, continuation,
                (uint32_t)sizeof(response), &response, &eaxValue, &continuation, &actualSize, &error);

            if (!callOk || eaxValue != 0x534D4150) {
                // TODO: PANIC
                F_FAIL_ASSERT("call not ok");
            }
        }

        const char* GetTypeString(Bootparams::MemoryRegionType type) {
            using namespace Bootparams;

            switch (type) {
                case MemoryRegionType::AvailableMemory:
                    return "Available";
                case MemoryRegionType::ReservedMemory:
                    return "Reserved";
                case MemoryRegionType::ACPIReclaimMemory:
                    return "ACPI Reclaimable";
                case MemoryRegionType::ACPINVSMemory:
                    return "ACPI NVS";
                default:
                    return "?? UNKNOWN";
            }
        }

        Bootparams::MemoryRegionType MapE820Type(uint32_t type) {
            switch (type) {
                case 0x01:
                    return Bootparams::MemoryRegionType::AvailableMemory;
                case 0x02:
                    return Bootparams::MemoryRegionType::ReservedMemory;
                case 0x03:
                    return Bootparams::MemoryRegionType::ACPIReclaimMemory;
                case 0x04:
                    return Bootparams::MemoryRegionType::ACPINVSMemory;
                default:
                    return Bootparams::MemoryRegionType::Unknown;
            }
        }

    }  // namespace

    void InitializeHighMemory() {
        // Reset map to default state
        GetMemoryMap().Clear();
        GetMemoryMap().EnsureCapacity(16);

        // Initialize memory map with high memory allocators
        uint32_t continuation = 0;

        do {
            E820Response e820Response;
            CallE820(e820Response, continuation);

            Bootparams::MemoryRegion& region = GetMemoryMap().AppendInPlace();

            region.RegionStart = e820Response.BaseAddress;
            region.RegionEnd   = e820Response.BaseAddress + e820Response.Length;
            region.Type        = MapE820Type(e820Response.E820Type);
        } while (continuation != 0);

        RemoveOverlappingRegions();
        MergeAdjacentMemoryRegions();

        // Initially only the first 1GB of physical memory is mapped so we cannot now initialize blocks that span
        // beyond the 1 GB boundary, instead any block that crosses that boundary is split into half, the lower part
        // below this boundary is marked as available and the higher part as LongMemReclaimable.
        constexpr const Bootparams::physicaladdress_t BOUNDARY_1GB = 1024ULL * 1024ULL * 1024ULL;

        for (auto& region : GetMemoryMap()) {
            if (region.Type != Bootparams::MemoryRegionType::AvailableMemory) {
                continue;
            }

            if (region.RegionStart >= BOUNDARY_1GB) {
                region.Type = Bootparams::MemoryRegionType::LongMemReclaimable;
                continue;
            }

            if (!region.IsInRegion(BOUNDARY_1GB)) {
                continue;
            }

            Bootparams::MemoryRegion highRegion = SplitRegion(region, BOUNDARY_1GB);

            highRegion.Type = Bootparams::MemoryRegionType::LongMemReclaimable;

            GetMemoryMap().Append(Stdlib::Move(highRegion));
        }

        // TODO: setup high memory allocators
    }

    void DumpMemoryMap() {
#ifdef F_DEBUG
        FB_LOG_DEBUG("E820 memory map: ");

        for (const auto& entry : GetMemoryMap()) {
            FB_LOG_DEBUG_F("\t%016llx -> %016llx T %s", entry.RegionStart, entry.RegionEnd, GetTypeString(entry.Type));
        }
#endif
    }
}  // namespace FunnyOS::Bootloader64