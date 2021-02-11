#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP

#include <FunnyOS/Stdlib/Vector.hpp>
#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootparams {

    /**
     * Type of a memory entry.
     */
    enum class MemoryRegionType : uint32_t {
        /**
         * Any other type that is not defined.
         */
        Unknown = 0x00,

        /**
         * Memory free to use
         */
        AvailableMemory = 0x01,

        /**
         * Reserved memory location. Do not use.
         */
        ReservedMemory = 0x02,

        /**
         * Usable after the OS reads ACPI tables.
         */
        ACPIReclaimMemory = 0x03,

        /**
         * OS is required to save this memory between NVS sessions
         */
        ACPINVSMemory = 0x04,

        /**
         * Reserved value, will never appear in an actual entry.
         */
        Reserved = 0x05,

        /**
         * Location of kernel image in the memory.
         */
        KernelImage = 0xA0,

        /**
         * Location at where the temporary bootloader's page tables are placed.
         *
         * May be reclaimed by kernel after switching to kernel page tables.
         */
        PageTableReclaimable = 0xA1,

        /**
         * Location that is available but not usable because it is outside of mapped physical memory.
         *
         * May be reclaimed by kernel after mapping the entire physical memory.
         */
        LongMemReclaimable = 0xA2,
    };

    /**
     * Represents an integer type used for storing physical addresses.
     *
     * Physical addresses can be accessed via PhysicalAddressToPointer.
     */
    using physicaladdress_t = uint64_t;

    /**
     * An entry in the memory map.
     */
    struct MemoryRegion {
        /**
         * Start of the region (inclusive)
         */
        uint64_t RegionStart;

        /**
         * End of the region (exclusive)
         */
        uint64_t RegionEnd;

        /**
         * Type of the memory.
         */
        MemoryRegionType Type;

        bool IsInRegion(uint64_t address) {
            return address >= RegionStart && address < RegionEnd;
        }
    };

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP
