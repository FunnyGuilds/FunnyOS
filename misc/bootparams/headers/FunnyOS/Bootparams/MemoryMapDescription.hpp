#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP

#include <FunnyOS/Stdlib/Vector.hpp>
#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootparams {

    /**
     * ACPI 3.0 Extended attributes flags for memory map entries.
     */
    namespace ACPI30Flags {
        /**
         * If it is not set the memory block should be ignored, as it it was reserved
         */
        constexpr static uint32_t DONT_IGNORE = 1 << 0;

        /**
         * According to the standard "Memory reported as non-volatile may require characterization to determine its
         * suitability for use as conventional RAM.". Whatever that means.
         */
        constexpr static uint32_t MEMORY_VOLATILE = 1 << 1;

    }  // namespace ACPI30Flags

    /**
     * Type of a memory entry.
     */
    enum class MemoryMapEntryType : uint32_t {
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
     * An entry in the memory map.
     */
    struct MemoryMapEntry {
        /**
         * Where the memory described by this entry starts.
         */
        uint64_t BaseAddress;

        /**
         * Length of the memory.
         */
        uint64_t Length;

        /**
         * Type of the memory.
         */
        MemoryMapEntryType Type;

        /**
         * ACPI extended flags of this memory region.
         *
         * This value should be read only when MemoryMapHasAcpiExtendedAttribute is true.
         * Otherwise it can be ignored.
         */
        uint32_t ACPIFlags;
    };

    struct MemoryMapDescription {
        /**
         * Whether or not the MemoryMapEntries will have valid ACPIFlags
         */
        bool HasAcpiExtendedAttribute;

        Stdlib::Vector<MemoryMapEntry> Entries;

    };

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_MEMORYMAPDESCRIPTION_HPP
