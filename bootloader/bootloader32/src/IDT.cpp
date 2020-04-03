#include "IDT.hpp"

namespace FunnyOS::Bootloader32::IDT {
    constexpr size_t INTERRUPT_COUNT = 256;

    static IDTR* g_idtRegister = nullptr;
    static IDTDescriptor* g_idtDescriptor = nullptr;

    void LoadIdtr() {
        // cleanup old values if they exist
        delete g_idtRegister;
        delete[] g_idtDescriptor;

        g_idtRegister = new IDTR;
        g_idtDescriptor = new IDTDescriptor[INTERRUPT_COUNT];

        for (size_t i = 0; i < INTERRUPT_COUNT; i++) {
            auto* descriptor = (g_idtDescriptor + i);

            // TODO
        }

        g_idtRegister->Length = INTERRUPT_COUNT * sizeof(IDTDescriptor);
        g_idtRegister->Offset = reinterpret_cast<uint32_t>(g_idtDescriptor);

        __asm__ __volatile__("lidt [%0]" ::"g"(g_idtRegister));
    }
}  // namespace FunnyOS::Bootloader32::IDT