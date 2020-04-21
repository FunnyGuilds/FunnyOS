#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADERPARAMETERS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADERPARAMETERS_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootloader32 {

    struct BootloaderParameters {

        /**
         * ACPI flag. If it is not set the memory block should be ignored, as it it was reserved
         */
        constexpr static uint32_t ACPI_EXT_DONT_IGNORE_BIT = 1 << 0;

        /**
         * ACPI flag. According to the standard "Memory reported as non-volatile may require characterization to
         * determine its suitability for use as conventional RAM.". Whatever that means.
         */
        constexpr static uint32_t ACPI_EXT_MEMORY_VOLATILE = 1 << 1;

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
             * Usable after the ACPI tables if that location are no longer needed.
             */
            ACPIReclaimMemory = 0x03,

            /**
             * OS is required to save this memory between NVS sessions
             */
            ACPINVSMemory = 0x04,

            /**
             * Reserved value, will never appear in an actual entry.
             */
            Reserved = 0x05
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
             * This value should be read only when MemoryMapHasAcpiExtendedAttribute is true.
             * Otherwise it can be ignored.
             */
            uint32_t ACPIFlags;
        } F_DONT_ALIGN;

        /**
         * Number of the drive that the we are booting from.
         */
        uint8_t BootDriveNumber;

        /**
         * Number of the drive that the we are booting from.
         */
        uint8_t BootPartition;

        /**
         * Whether or not the MemoryMapEntry will have a valid ACPIFlags
         */
        bool MemoryMapHasAcpiExtendedAttribute;

        /**
         * Address of the first element of the memory map.
         */
        uint32_t MemoryMapStart;

        /**
         * Number of entries in the memory map.
         */
        uint16_t MemoryMapEntriesCount;
    } F_DONT_ALIGN;

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADERPARAMETERS_HPP
