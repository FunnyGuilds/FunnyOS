#include <FunnyOS/Hardware/RealMode.hpp>

#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/Stdlib/Compiler.hpp>

extern FunnyOS::HW::GDTInfo g_gdtInfo;
extern FunnyOS::HW::Registers16 g_savedRegisters;
extern uint8_t g_realBuffer;
extern uint8_t g_realBufferTop;

F_CDECL extern void do_real_mode_interrupt();

// TODO: Synchronization of some sort.

namespace FunnyOS::HW {
    Register16::Register16(const Register16& other) : Value16(other.Value16) {}

    Register16& Register16::operator=(const Register16& other) {
        Value16 = other.Value16;
        return *this;
    }

    Register16::Register16(uint16_t value) : Value16(value) {}

    void SetupRealModeInterrupts(GDTInfo gdtInfo) {
        g_gdtInfo = gdtInfo;
    }

    Memory::SizedBuffer<uint8_t>& GetRealModeBuffer() {
        static Memory::SizedBuffer<uint8_t> c_realModeBuffer{
            &g_realBuffer, reinterpret_cast<size_t>(&g_realBufferTop) - reinterpret_cast<size_t>(&g_realBuffer)};

        return c_realModeBuffer;
    }

    void GetRealModeBufferAddress(uint16_t& segment, uint16_t& offset) {
        auto address = reinterpret_cast<uintptr_t>(GetRealModeBuffer().Data);

        segment = address / 0x10000;
        offset = address % 0x10000;
    }

    void RealModeInt(uint8_t interrupt, Registers16& registers) {
        HW::NoInterruptsBlock noInterrupts;

#ifdef F_64
#else
        g_savedRegisters = registers;
#ifdef __GNUC__
        asm(
            // Save state
            "pushfd\n"
            "pushad\n"
            "push es\n"
            "push fs\n"
            "push gs\n"

            // Call do_real_mode_interrupt
            "push eax\n"
            "call do_real_mode_interrupt\n"
            "add esp, 4\n"

            // Restore state
            "pop gs\n"
            "pop fs\n"
            "pop es\n"
            "popad\n"
            "popfd\n"
            :
            : "a"(static_cast<uintmax_t>(interrupt)));
#endif
#endif

        registers = g_savedRegisters;
    }

}  // namespace FunnyOS::HW