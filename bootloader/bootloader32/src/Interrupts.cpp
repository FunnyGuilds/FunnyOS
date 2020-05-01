#include "Interrupts.hpp"

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/Hardware/PIC.hpp>
#include <FunnyOS/Hardware/PS2.hpp>

#include "Bootloader32.hpp"
#include "DebugMenu.hpp"
#include "InterruptSetup.hpp"
#include "Logging.hpp"
#include "Sleep.hpp"

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    namespace {
        bool g_isPausing = false;

        InterruptHandler g_interruptHandlers[HW::INTERRUPTS_COUNT];
        InterruptHandler g_unknownInterruptHandler;

        void InterruptHandlerSelector(InterruptData* data) {
            const HW::InterruptType type = data->Type;
            InterruptHandler handler = g_interruptHandlers[static_cast<int>(data->Type)];

            if (handler == nullptr) {
                handler = g_unknownInterruptHandler;
            }

            handler(data);

            // Send PIC End of interrupt command.
            if (HW::PIC::IsPICInterrupt(type)) {
                if (!HW::PIC::IsMasterPICInterrupt(type)) {
                    HW::PIC::SendEndOfInterrupt(false);
                }

                HW::PIC::SendEndOfInterrupt(true);
            }
        }
    }  // namespace

    inline void DecodeEflagPart(String::StringBuffer& buf, Register flags, HW::CPU::Flags flag, const char* str) {
        if ((flags & static_cast<uintmax_t>(flag)) != 0) {
            String::Concat(buf, buf.Data, str);
            String::Concat(buf, buf.Data, " ");
        }
    }

    void DecodeEflags(String::StringBuffer& buffer, Register eflags) {
        using HW::CPU::Flags;

        DecodeEflagPart(buffer, eflags, Flags::CarryFlag, "CF");
        DecodeEflagPart(buffer, eflags, Flags::ParityFlag, "PF");
        DecodeEflagPart(buffer, eflags, Flags::AdjustFlag, "AF");
        DecodeEflagPart(buffer, eflags, Flags::ZeroFlag, "ZF");
        DecodeEflagPart(buffer, eflags, Flags::SignFlag, "SF");
        DecodeEflagPart(buffer, eflags, Flags::TrapFlag, "TF");
        DecodeEflagPart(buffer, eflags, Flags::InterruptFlag, "IF");
        DecodeEflagPart(buffer, eflags, Flags::DirectionFlag, "DF");
        DecodeEflagPart(buffer, eflags, Flags::OverflowFlag, "OF");
        DecodeEflagPart(buffer, eflags, Flags::IOPL_Bit0, "IO");
        DecodeEflagPart(buffer, eflags, Flags::IOPL_Bit2, "PL");
        DecodeEflagPart(buffer, eflags, Flags::NestedTaskFlag, "NT");
        DecodeEflagPart(buffer, eflags, Flags::ResumeFlag, "RF");
        DecodeEflagPart(buffer, eflags, Flags::Virtual8086_ModeFlag, "VM");
        DecodeEflagPart(buffer, eflags, Flags::AlignmentCheck, "AC");
        DecodeEflagPart(buffer, eflags, Flags::VirtualInterruptFlag, "VIF");
        DecodeEflagPart(buffer, eflags, Flags::VirtualInterruptPending, "VIP");
        DecodeEflagPart(buffer, eflags, Flags::CPUID_Supported, "ID");
    }

    void UnknownInterruptHandler(InterruptData* data) {
        char interrupt[20];
        String::StringBuffer interruptBuffer = {interrupt, 20};
        char panic[384];
        String::StringBuffer panicBuffer = {panic, 384};

        Optional<const char*> mnemonic = HW::GetInterruptMnemonic(data->Type);

        bool formatOk;
        if (mnemonic) {
            formatOk = String::Format(interruptBuffer, "%s(%u)", mnemonic.GetValue(), data->ErrorCode);
        } else {
            formatOk = String::Format(interruptBuffer, "0x%02X(%u)", static_cast<int>(data->Type), data->ErrorCode);
        }

        if (!formatOk) {
            Bootloader::Get().Panic("Interrupt info collection failed");
        }

        char eflags[33] = {0};
        String::StringBuffer eflagsBuffer{eflags, 33};
        String::IntegerToString(eflagsBuffer, data->EFLAGS, 2);

        char decodedEflags[43] = {0};
        String::StringBuffer decodedEflagsBuffer{decodedEflags, 43};
        DecodeEflags(decodedEflagsBuffer, data->EFLAGS);

        formatOk = String::Format(panicBuffer,
                                  "An unexpected interrupt has occurred: %s\r\n"
                                  "    Register dump:\r\n"
                                  "        EAX = 0x%08x ECX = 0x%08x EDX = 0x%08x EBX = 0x%08x\r\n"
                                  "        ESP = 0x%08x EBP = 0x%08x ESI = 0x%08x EDI = 0x%08x\r\n"
                                  "        EIP = 0x%08x \r\n"
                                  "    EFLAGS:\r\n"
                                  "        Raw value = 0b%032s\r\n"
                                  "        Decoded = %s\r\n",
                                  interrupt, data->EAX, data->ECX, data->EDX, data->EBX, data->ESP, data->EBP,
                                  data->ESI, data->EDI, data->EIP, eflags, decodedEflags);

        if (!formatOk) {
            Bootloader::Get().Panic("Interrupt info collection failed");
        }

        Bootloader::Get().Panic(panicBuffer.Data);
    }

    void Env64InterruptHandler(InterruptData* data) {
        if (data->EAX == 0x01) {
            FB_LOG_DEBUG_F("ENV64 message: %s", data->ESI);
            return;
        }

        String::StringBuffer buffer = Memory::AllocateBuffer<char>(64);
        String::Format(buffer, "ENV 64 interrupt. Code: 0x%02x. Detail: 0x%02x", data->EAX, data->EDX);
        Bootloader::Get().Panic(buffer.Data);
    }

    void KeyboardHandler(InterruptData* data) {
        using HW::PS2::ScanCode;

        ScanCode scanCode;
        while (HW::PS2::TryReadScanCode(scanCode)) {
            if (scanCode == ScanCode::Pause_Pressed) {
                FB_LOG_WARNING_F("Execution paused at EIP = 0x%04x", data->EIP);
                FB_LOG_WARNING("Press enter to continue ...");

                while (!HW::PS2::TryReadScanCode(scanCode) || scanCode != ScanCode::Enter_Released) {
                    // Wait
                }

                FB_LOG_OK("Continuing");
                continue;
            }

            if (g_isPausing && scanCode == ScanCode::Enter_Released) {
                g_isPausing = false;
                continue;
            }

            DebugMenu::HandleKey(scanCode);
        }
    }

    void SetupInterrupts() {
        RegisterUnknownInterruptHandler(&UnknownInterruptHandler);

        RegisterInterruptHandler(HW::InterruptType::Env64Interrupt, &Env64InterruptHandler);
        RegisterInterruptHandler(HW::InterruptType::IRQ_PIT_Interrupt, &PITInterruptHandler);
        RegisterInterruptHandler(HW::InterruptType::IRQ_KeyboardInterrupt, &KeyboardHandler);

        SetupInterruptTable(&InterruptHandlerSelector);

        HW::PIC::Remap();
        HW::PIC::SetEnabledInterrupts(0b111);

        HW::EnableHardwareInterrupts();
        HW::EnableNonMaskableInterrupts();
    }

    void RegisterInterruptHandler(HW::InterruptType type, InterruptHandler handler) {
        g_interruptHandlers[static_cast<int>(type)] = handler;
    }

    void UnregisterInterruptHandler(HW::InterruptType type) {
        RegisterInterruptHandler(type, nullptr);
    }

    void RegisterUnknownInterruptHandler(InterruptHandler handler) {
        g_unknownInterruptHandler = handler;
    }

    void Pause() {
        g_isPausing = true;

        while (g_isPausing) {
            HW::CPU::Halt();
        }
    }
}  // namespace FunnyOS::Bootloader32
