#include "MemoryMap.hpp"

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/CPU.hpp>

#include "RealMode.hpp"

namespace FunnyOS::Bootloader32 {
    int CreateMemoryMap(Bootparams::MemoryMapDescription& map) {
        map.HasAcpiExtendedAttribute = false;

        Registers32 registers;
        registers.EBX.Value32 = 0;

        auto* entries = reinterpret_cast<Bootparams::MemoryMapEntry*>(0x500);
        auto* currentEntry = reinterpret_cast<Bootparams::MemoryMapEntry*>(GetRealModeBuffer().Data);

        size_t i;
        for (i = 0;; i++) {
            GetRealModeBufferAddress(registers.ES.Value16, registers.EDI.Value16);
            registers.EAX.Value32 = 0x0000E820;
            registers.ECX.Value32 = sizeof(*currentEntry);
            registers.EDX.Value32 = 0x534D4150;

            RealModeInt(0x15, registers);
            if ((registers.EFLAGS.Value16 & static_cast<uint16_t>(HW::CPU::Flags::CarryFlag)) != 0) {
                // Fail
                return registers.EAX.Value8.High;
            }

            if (registers.ECX.Value32 >= 24) {
                map.HasAcpiExtendedAttribute = true;
            } else {
                currentEntry->ACPIFlags = 0b01;
            }

            entries[i] = *currentEntry;

            if (registers.EBX.Value32 == 0) {
                break;
            }
        }

        map.First = entries;
        map.Count = static_cast<uint16_t>(i);

        return 0;
    }

    Bootparams::MemoryMapEntry FindBiggestUsableMemoryEntry(const Bootparams::MemoryMapDescription& memoryMap) {
        constexpr const uint64_t MINIMUM_ADDRESSABLE_BYTE = 1024ULL * 1024ULL * 2ULL;  // 2 MB
        constexpr const uint64_t MAXIMUM_ADDRESSABLE_BYTE = Stdlib::NumeralTraits::Info<uint32_t>::MaximumValue;

        Bootparams::MemoryMapEntry biggestEntry{0, 0, Bootparams::MemoryMapEntryType::Reserved, 0};

        for (size_t i = 0; i < memoryMap.Count; i++) {
            const auto& entry = memoryMap.First[i];

            if (entry.Type != Bootparams::MemoryMapEntryType::AvailableMemory) {
                continue;
            }

            if (memoryMap.HasAcpiExtendedAttribute) {
                if ((entry.ACPIFlags & Bootparams::ACPI30Flags::DONT_IGNORE) == 0) {
                    continue;
                }

                if ((entry.ACPIFlags & Bootparams::ACPI30Flags::MEMORY_VOLATILE) != 0) {
                    continue;
                }
            }

            if (entry.BaseAddress > MAXIMUM_ADDRESSABLE_BYTE) {
                continue;
            }

            // Found suitable memory, now check for size and boundaries
            uint64_t entryMinimumByte = entry.BaseAddress;
            uint64_t entryMaximumByte = entry.BaseAddress + entry.Length - 1;

            // Base address must be page-aligned
            if ((entryMinimumByte % 0x1000) != 0) {
                entryMinimumByte += 0x1000 - (entryMinimumByte % 0x1000);
            }

            // We cannot use addresses larger than 32-bit in 32-bit mode.
            if (entryMaximumByte > MAXIMUM_ADDRESSABLE_BYTE) {
                entryMaximumByte = MAXIMUM_ADDRESSABLE_BYTE;
            }

            // We cannot use addresses < than 2 MB since they will be identity mapped
            if (entryMinimumByte < MINIMUM_ADDRESSABLE_BYTE) {
                entryMinimumByte = MINIMUM_ADDRESSABLE_BYTE;
            }

            if (entryMinimumByte > entryMaximumByte) {
                // Whole entry is below minimum or above maximum
                continue;
            }

            const uint64_t totalUsableSpace = entryMaximumByte - entryMinimumByte + 1;

            // Found suitable memory area
            if (biggestEntry.Length < totalUsableSpace) {
                biggestEntry.BaseAddress = entryMinimumByte;
                biggestEntry.Length = totalUsableSpace;
                biggestEntry.Type = entry.Type;
                biggestEntry.ACPIFlags = entry.ACPIFlags;
            }
        }

        return biggestEntry;
    }
}  // namespace FunnyOS::Bootloader32