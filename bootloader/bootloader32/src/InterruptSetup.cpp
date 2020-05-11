#include "InterruptSetup.hpp"

// declared in interrupt_setup.asm
extern uint32_t g_firstRoutineAddress;
extern uint32_t g_interruptRoutineSize;

namespace FunnyOS::Bootloader32 {
    namespace {
        /**
         * Interrupt handler
         */
        InterruptHandler g_handler;

        /**
         * Location of the IDT in memory.
         */
        InterruptGateDescriptor g_idt[HW::INTERRUPTS_COUNT];

        void SetupInterruptGateDescriptor(size_t i, InterruptGateDescriptor* descriptor) {
            // Flags for the entries
            static const uint8_t c_flags = (0b110 << 0) |  // Entry type: 0b110 = Interrupt gate type
                                           (1 << 3) |      // Size of gate: 1 = 32 bits
                                           (0 << 4) |      // Storage segment, only for task gates
                                           (0 << 5) |      // Descriptor privilege level (2 bits)
                                           (1 << 7);       // Present flag, must be set for all used descriptors

            // Segment selector for the entries
            const uint32_t offset = g_firstRoutineAddress + i * g_interruptRoutineSize;

            descriptor->OffsetLow = static_cast<uint16_t>(offset & 0xFFFF);
            descriptor->SegmentSelector = 0x08;
            descriptor->Reserved = 0;
            descriptor->Flags = c_flags;
            descriptor->OffsetHigh = static_cast<uint16_t>((offset >> 16) & 0xFFFF);
        }

        void LoadIDT() {
            IDTR idt{static_cast<uint16_t>(HW::INTERRUPTS_COUNT * g_interruptRoutineSize - 1),
                     reinterpret_cast<uint32_t>(g_idt)};

#ifdef __GNUC__
            asm volatile(
                "lidt %0"
                :
                : "m"(idt));
#endif
        }
    }  // namespace

    void SetupInterruptTable(InterruptHandler handler) {
        // Setup handler
        g_handler = handler;

        // Allocate IDT
        for (size_t i = 0; i < HW::INTERRUPTS_COUNT; i++) {
            SetupInterruptGateDescriptor(i, g_idt + i);
        }

        LoadIDT();
    }

}  // namespace FunnyOS::Bootloader32

// Called from interrupt_setup.asm
extern "C" void F_CDECL interrupt_routine(FunnyOS::Bootloader32::InterruptData data) {
    FunnyOS::Bootloader32::g_handler(&data);
}