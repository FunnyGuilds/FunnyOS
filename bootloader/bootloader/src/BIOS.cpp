#include "BIOS.hpp"

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>
#include "Bootloader.hpp"

struct BiosState {
    // Registers
    uint32_t EAX, ECX, EDX, EBX, EDI, ESI, EBP, EFLAGS, ES, FS, GS;

    // Selectors
    uint16_t Selector64BitCode, Selector64BitData, Selector16BitCode, Selector16BitData;

    // Interrupt number
    uint8_t InterruptNumber;
};

extern BiosState g_biosState;  // bios.asm
extern "C" void do_call_bios();

namespace FunnyOS::Bootloader64::BIOS {
    using namespace Stdlib::String;

    namespace {
        void SetRegisterIfMatches(
            const char* token, int value, uint32_t& reg, const char* name32bit, const char* name16bit,
            const char* name8bitHigh, const char* name8bitLow) {
            if (name32bit && Compare(token, name32bit) == 0) {
                reg = static_cast<uint32_t>(value & 0xFFFFFFFF);
            } else if (name16bit && Compare(token, name16bit) == 0) {
                reg = static_cast<uint32_t>(value & 0xFFFF);
            } else if (name8bitHigh && Compare(token, name8bitHigh) == 0) {
                const auto value8 = static_cast<uint32_t>(value & 0xFF);

                reg = (reg & 0xFFFF00FF) | (value8 << 8);
            } else if (name8bitLow && Compare(token, name8bitLow) == 0) {
                const auto value8 = static_cast<uint32_t>(value & 0xFF);

                reg = (reg & 0xFFFFFF00) | value8;
            }
        }

        void GetRegisterIfMatches(
            const char* token, void* ptr, uint32_t& reg, const char* name32bit, const char* name16bit,
            const char* name8bitHigh, const char* name8bitLow) {
            if (Compare(token, name32bit) == 0) {
                *static_cast<uint32_t*>(ptr) = reg & 0xFFFFFFFF;
            } else if (Compare(token, name16bit) == 0) {
                *static_cast<uint16_t*>(ptr) = reg & 0x0000FFFF;
            } else if (Compare(token, name8bitHigh) == 0) {
                *static_cast<uint8_t*>(ptr) = ((reg & 0x0000FF00) >> 8);
            } else if (Compare(token, name8bitLow) == 0) {
                *static_cast<uint8_t*>(ptr) = reg & 0x000000FF;
            }
        }

    }  // namespace

    uint16_t GetRealModeSegment(void* address) {
        return (reinterpret_cast<uintptr_t>(address) & 0xFFFF0) >> 4;
    }

    uint16_t GetRealModeOffset(void* address) {
        return (reinterpret_cast<uintptr_t>(address) & 0x0000F) >> 0;
    }

    void CallBios(uint8_t interrupt, const char* format, ...) {
        va_list inputArguments;
        va_list outputArguments;

        va_start(inputArguments, format);
        va_copy(outputArguments, inputArguments);

        // Copy format string
        size_t formatLength = Length(format);
        Stdlib::SmartSizedBuffer<char> formatCopyBuffer{formatLength};
        Stdlib::Memory::Copy(formatCopyBuffer, format);

        char* formatCopy = formatCopyBuffer.Data;

        // Parse it
        while (formatCopy != nullptr) {
            char* token = NextToken(&formatCopy, ",");
            Trim(&token);
            ToLowercase(token);

            if (*token == '=') {
                // output parameter, ignore for now
                va_arg(inputArguments, void*);
                continue;
            }

            int value = va_arg(inputArguments, int);

            SetRegisterIfMatches(token, value, g_biosState.EAX, "eax", "ax", "ah", "al");
            SetRegisterIfMatches(token, value, g_biosState.ECX, "ecx", "cx", "ch", "cl");
            SetRegisterIfMatches(token, value, g_biosState.EDX, "edx", "dx", "dh", "dl");
            SetRegisterIfMatches(token, value, g_biosState.EBX, "ebx", "bx", "bh", "bl");
            SetRegisterIfMatches(token, value, g_biosState.EDI, "edi", "di", "doh", "dol");
            SetRegisterIfMatches(token, value, g_biosState.ESI, "esi", "si", "soh", "sol");
            SetRegisterIfMatches(token, value, g_biosState.EBP, "ebp", "bp", nullptr, nullptr);
            SetRegisterIfMatches(token, value, g_biosState.EFLAGS, "eflags", "flags", nullptr, nullptr);
            SetRegisterIfMatches(token, value, g_biosState.ES, nullptr, "es", nullptr, nullptr);
            SetRegisterIfMatches(token, value, g_biosState.FS, nullptr, "fs", nullptr, nullptr);
            SetRegisterIfMatches(token, value, g_biosState.GS, nullptr, "gs", nullptr, nullptr);
        }
        va_end(inputArguments);

        // non-register parameters
        g_biosState.Selector64BitCode = GDT_SELECTOR_CODE64 << 3;
        g_biosState.Selector64BitData = GDT_SELECTOR_DATA64 << 3;
        g_biosState.Selector16BitCode = GDT_SELECTOR_CODE16 << 3;
        g_biosState.Selector16BitData = GDT_SELECTOR_DATA16 << 3;
        g_biosState.InterruptNumber   = interrupt;

        do_call_bios();
        F_MEMORY_FENCE;

        // Set output parameters
        Stdlib::Memory::Copy(formatCopyBuffer, format);
        formatCopy = formatCopyBuffer.Data;

        while (formatCopy != nullptr) {
            char* token = NextToken(&formatCopy, ",");
            Trim(&token);
            ToLowercase(token);

            if (*token != '=') {
                // input parameter, ignore
                va_arg(outputArguments, int);
                continue;
            }

            token++;  // skip '='

            void* ptr = va_arg(outputArguments, void*);

            GetRegisterIfMatches(token, ptr, g_biosState.EAX, "eax", "ax", "ah", "al");
            GetRegisterIfMatches(token, ptr, g_biosState.ECX, "ecx", "cx", "ch", "cl");
            GetRegisterIfMatches(token, ptr, g_biosState.EDX, "edx", "dx", "dh", "dl");
            GetRegisterIfMatches(token, ptr, g_biosState.EBX, "ebx", "bx", "bh", "bl");
            GetRegisterIfMatches(token, ptr, g_biosState.EDI, "edi", "di", "doh", "dol");
            GetRegisterIfMatches(token, ptr, g_biosState.ESI, "esi", "si", "soh", "sol");
            GetRegisterIfMatches(token, ptr, g_biosState.EBP, "ebp", "bp", nullptr, nullptr);
            GetRegisterIfMatches(token, ptr, g_biosState.EFLAGS, "eflags", "flags", nullptr, nullptr);
            GetRegisterIfMatches(token, ptr, g_biosState.ES, nullptr, "es", nullptr, nullptr);
            GetRegisterIfMatches(token, ptr, g_biosState.FS, nullptr, "fs", nullptr, nullptr);
            GetRegisterIfMatches(token, ptr, g_biosState.GS, nullptr, "gs", nullptr, nullptr);
        }

        va_end(outputArguments);
    }

}  // namespace FunnyOS::Bootloader64::BIOS
