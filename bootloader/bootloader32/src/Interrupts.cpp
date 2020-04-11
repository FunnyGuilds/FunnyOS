#include "Interrupts.hpp"

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PIC.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Sleep.hpp>

#include "Bootloader32.hpp"

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    inline void DecodeEflagPart(String::StringBuffer& buf, HW::Register flags, HW::CPU::Flags flag, const char* str) {
        if ((flags & flag) != 0) {
            String::Concat(buf, buf.Data, str);
            String::Concat(buf, buf.Data, " ");
        }
    }

    void DecodeEflags(String::StringBuffer& buffer, HW::Register eflags) {
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
        DecodeEflagPart(buffer, eflags, Flags::AlingmentCheck, "AC");
        DecodeEflagPart(buffer, eflags, Flags::VirtualInterruptFlag, "VIF");
        DecodeEflagPart(buffer, eflags, Flags::VirtualInterruptPending, "VIP");
        DecodeEflagPart(buffer, eflags, Flags::CPUID_Supported, "ID");
    }

    void UnknownInterruptHandler(HW::InterruptData* data) {
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
            Bootloader::GetBootloader()->Panic("Interrupt info collection failed");
        }

        char eflags[33];
        String::StringBuffer eflagsBuffer{eflags, 33};
        String::IntegerToString(eflagsBuffer, data->EFLAGS, 2);

        char decodedEflags[43];
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
            Bootloader::GetBootloader()->Panic("Interrupt info collection failed");
        }

        Bootloader::GetBootloader()->Panic(panicBuffer.Data);
    }

    void Keyboardhandler(HW::InterruptData* data) {
        HW::InputOutput::InputByte(0x64);
        HW::InputOutput::InputByte(0x60);
        FB_LOG_WARNING("Keyboard interrupt!");
    }

    void NoOpHandler(HW::InterruptData*) {
    }

    void SetupInterrupts() {
        HW::RegisterUnknownInterruptHandler(&UnknownInterruptHandler);

        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_PIT_Interrupt, &Bootloader::PITInterruptHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_KeyboardInterrupt, &Keyboardhandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_CascadeInterrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_COM2_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_COM1_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_LPT2_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_FloppyInterrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_LPT1_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_CMOS_RealTimeClockInterrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_ACPI_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_10_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_11_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_PS2_MouseInterrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_CoProcessor_FPU_IPI_Interrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_PrimaryAtaHardDriveInterrupt, &NoOpHandler);
        HW::RegisterInterruptHandler(HW::InterruptType ::IRQ_SecondaryAtaHardDriveInterrupt, &NoOpHandler);

        HW::SetupInterrupts();
        HW::PIC::Remap();
        HW::EnableHardwareInterrupts();
        HW::EnableNonMaskableInterrupts();
    }
}  // namespace FunnyOS::Bootloader32
