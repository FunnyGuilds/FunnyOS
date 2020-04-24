#include "InterruptSetup.hpp"

extern "C" void setup_routines(FunnyOS::HW::InterruptSetup::InterruptRoutinesData* data) F_CDECL;

namespace FunnyOS::HW::InterruptSetup {
    namespace {
        /**
         * Interrupt handler
         */
        InterruptHandler g_handler;

        /**
         * Location of the IDT in memory.
         */
        InterruptGateDescriptor* g_idt = nullptr;

        InterruptRoutinesData g_interruptRoutinesData;

        // Called from interrupt_setup.asm
        void F_CDECL interrupt_routine(FunnyOS::HW::InterruptData data) {
            FunnyOS::HW::InterruptSetup::g_handler(&data);
        }
    }  // namespace

    const InterruptRoutinesData& GetInterruptRoutinesData() {
        return g_interruptRoutinesData;
    }

    uint16_t CreateSegmentSelector(uint8_t rpl, SegmentSelectorLocation location, uint16_t index) {
        return (rpl << 0) | (static_cast<uint8_t>(location) << 2) | (index << 3);
    }

    namespace {
#ifdef F_64
        void SetupInterruptGateDescriptor(size_t i, InterruptGateDescriptor* descriptor) {}

        void LoadIDT() {}
#else
        void SetupInterruptGateDescriptor(size_t i, InterruptGateDescriptor* descriptor) {
            // Flags for the entries
            static const uint8_t c_flags = (0b110 << 0) |  // Entry type: 0b110 = Interrupt gate type
                                           (1 << 3) |      // Size of gate: 1 = 32 bits
                                           (0 << 4) |      // Storage segment, only for task gates
                                           (0 << 5) |      // Descriptor privilege level (2 bits)
                                           (1 << 7);       // Present flag, must be set for all used descriptors

            // Segment selector for the entries
            static const SegmentSelector c_segmentSelector = CreateSegmentSelector(0, SegmentSelectorLocation::GDT, 1);

            const uint32_t offset =
                g_interruptRoutinesData.FirstRoutineAddress + i * g_interruptRoutinesData.InterruptRoutineSize;

            descriptor->OffsetLow = static_cast<uint16_t>(offset & 0xFFFF);
            descriptor->SegmentSelector = c_segmentSelector;
            descriptor->Reserved = 0;
            descriptor->Flags = c_flags;
            descriptor->OffsetHigh = static_cast<uint16_t>((offset >> 16) & 0xFFFF);
        }

        void LoadIDT() {
            IDT idt{static_cast<uint16_t>(sizeof(InterruptGateDescriptor) * INTERRUPTS_COUNT - 1),
                    reinterpret_cast<uint32_t>(g_idt)};

#ifdef __GNUC__
            asm volatile("lidt [%0]" : : "m"(idt));
#endif
        }
#endif
    }  // namespace

    void SetupInterruptTable(InterruptHandler handler) {
        g_interruptRoutinesData.InterruptHandlerAddress = reinterpret_cast<uintmax_t>(&interrupt_routine);
        setup_routines(&g_interruptRoutinesData);

        // Setup handler
        g_handler = handler;

        // Allocate IDT
        if (g_idt == nullptr) {
            g_idt = new InterruptGateDescriptor[INTERRUPTS_COUNT];
        }

        for (size_t i = 0; i < INTERRUPTS_COUNT; i++) {
            SetupInterruptGateDescriptor(i, g_idt + i);
        }

        LoadIDT();
    }

}  // namespace FunnyOS::HW::InterruptSetup
