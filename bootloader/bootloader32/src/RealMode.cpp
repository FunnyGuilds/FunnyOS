#include "RealMode.hpp"

#include <FunnyOS/Hardware/Interrupts.hpp>

// Defined in real_mode.asm
extern FunnyOS::Bootloader32::Registers32 g_savedRegisters;
extern uint8_t g_realBuffer;
extern uint8_t g_realBufferTop;

F_CDECL extern void do_real_mode_interrupt();

namespace FunnyOS::Bootloader32 {
    using namespace Stdlib;

    constexpr static uint32_t MAX_REAL_MODE_ADDR = 0xFFFF * 16 + 0xFFFF;

    Memory::SizedBuffer<uint8_t>& GetRealModeBuffer() {
        static Memory::SizedBuffer<uint8_t> c_realModeBuffer{
            &g_realBuffer, reinterpret_cast<size_t>(&g_realBufferTop) - reinterpret_cast<size_t>(&g_realBuffer)};

        return c_realModeBuffer;
    }

    void GetRealModeAddress(void* address, uint16_t& segment, uint16_t& offset) {
        GetRealModeAddress(reinterpret_cast<uint32_t>(address), segment, offset);
    }

    void GetRealModeAddress(uint32_t address, uint16_t& segment, uint16_t& offset) {
        F_ASSERT(address < MAX_REAL_MODE_ADDR, "address >= MAX_REAL_MODE_ADDR");

        segment = static_cast<uint16_t>((address & 0xF0000) >> 4);
        offset = static_cast<uint16_t>(address & 0xFFFF);
    }

    void GetRealModeBufferAddress(uint16_t& segment, uint16_t& offset) {
        auto address = reinterpret_cast<uintptr_t>(GetRealModeBuffer().Data);
        GetRealModeAddress(address, segment, offset);
    }

    void RealModeInt(uint8_t interrupt, Registers32& registers) {
        HW::NoInterruptsBlock noInterrupts;
        Memory::Copy(static_cast<void*>(&g_savedRegisters), static_cast<void*>(&registers), sizeof(Registers32));

#ifdef __GNUC__
        asm(
            // Save state
            "pushfl\n"
            "pushal\n"
            "pushw %%es\n"
            "pushw %%fs\n"
            "pushw %%gs\n"

            // Call do_real_mode_interrupt
            "push %%eax\n"
            "call do_real_mode_interrupt\n"
            "add $4, %%esp\n"

            // Restore state
            "popw %%gs\n"
            "popw %%fs\n"
            "popw %%es\n"
            "popal\n"
            "popfl\n"
            :
            : "a"(static_cast<uintmax_t>(interrupt))
            : "memory");
#endif

        Memory::Copy(static_cast<void*>(&registers), static_cast<void*>(&g_savedRegisters), sizeof(Registers32));
    }

}  // namespace FunnyOS::Bootloader32