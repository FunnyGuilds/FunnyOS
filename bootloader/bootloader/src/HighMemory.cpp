#include "HighMemory.hpp"

#include <FunnyOS/Hardware/BIOS.hpp>
#include "Bootloader.hpp"

namespace FunnyOS::Bootloader64 {

    Bootparams::MemoryMapDescription& GetMemoryMapDescription() {
        static Bootparams::MemoryMapDescription c_memoryMapDescription;

        return c_memoryMapDescription;
    }

    void AddMemoryMapEntry(Bootparams::MemoryMapEntry entry) {
        GetMemoryMapDescription().Entries.Append(Stdlib::Move(entry));
    }

    namespace {
        void CallE820(Bootparams::MemoryMapEntry& entry, uint32_t& continuation) {
            uint32_t eaxValue;
            uint32_t actualSize;
            uint32_t error;

            const bool callOk = HW::BIOS::CallBios(
                0x15, "eax, edx, ebx, ecx, es:di, =eax, =ebx, =ecx, =ah", 0x0000E820, 0x534D4150, continuation,
                (uint32_t)sizeof(entry), &entry, &eaxValue, &continuation, &actualSize, &error);

            if (!callOk || eaxValue != 0x534D4150) {
                // TODO: PANIC
                F_FAIL_ASSERT("call not ok");
            }

            if (actualSize > 20) {
                GetMemoryMapDescription().HasAcpiExtendedAttribute = true;
            }
        }

        const char* GetTypeString(Bootparams::MemoryMapEntryType type) {
            using namespace Bootparams;

            switch (type) {
                case MemoryMapEntryType::AvailableMemory:
                    return "Available";
                case MemoryMapEntryType::ReservedMemory:
                    return "Reserved";
                case MemoryMapEntryType::ACPIReclaimMemory:
                    return "ACPI Reclaimable";
                case MemoryMapEntryType::ACPINVSMemory:
                    return "ACPI NVS";
                default:
                    return "?? UNKNOWN";
            }
        }

    }  // namespace

    void InitializeHighMemory() {
        // Reset map to default state
        GetMemoryMapDescription().HasAcpiExtendedAttribute = false;

        auto& map = GetMemoryMapDescription().Entries;
        map.Clear();
        map.EnsureCapacity(16);

        // Initialize memory map with high memory allocators
        uint32_t continuation = 0;

        do {
            Bootparams::MemoryMapEntry& entry = map.AppendInPlace();
            CallE820(entry, continuation);

            // TODO: Remove overlaps?

            if (entry.Type != Bootparams::MemoryMapEntryType::AvailableMemory) {
                continue;
            }

        } while (continuation != 0);

        // TODO: setup high memory allocators
    }

    void DumpMemoryMap() {
#ifdef F_DEBUG
        FB_LOG_DEBUG("E820 memory map: ");

        for (const auto& entry : GetMemoryMapDescription().Entries) {
            FB_LOG_DEBUG_F("\tB %016x L %016x T %s", entry.BaseAddress, entry.Length, GetTypeString(entry.Type));
        }
#endif
    }
}  // namespace FunnyOS::Bootloader64