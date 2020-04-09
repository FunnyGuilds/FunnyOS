#include "Interrupts.hpp"

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/BootloaderCommons/Logging.hpp>

#include "Bootloader32.hpp"

namespace FunnyOS::Bootloader32 {
    using namespace FunnyOS::Stdlib;

    inline void DecodeEflagPart(String::StringBuffer& buffer, HW::Register eflags, int bit, const char* flag) {
        if ((eflags & (1 << bit)) != 0) {
            String::Concat(buffer, buffer.Data, flag);
            String::Concat(buffer, buffer.Data, " ");
        }
    }

    void DecodeEflags(String::StringBuffer& buffer, HW::Register eflags) {
        DecodeEflagPart(buffer, eflags, 0, "CF");
        DecodeEflagPart(buffer, eflags, 2, "PF");
        DecodeEflagPart(buffer, eflags, 4, "AF");
        DecodeEflagPart(buffer, eflags, 6, "ZF");
        DecodeEflagPart(buffer, eflags, 7, "SF");
        DecodeEflagPart(buffer, eflags, 8, "TF");
        DecodeEflagPart(buffer, eflags, 9, "IF");
        DecodeEflagPart(buffer, eflags, 10, "DF");
        DecodeEflagPart(buffer, eflags, 11, "OF");
        DecodeEflagPart(buffer, eflags, 12, "IO");
        DecodeEflagPart(buffer, eflags, 13, "PL");
        DecodeEflagPart(buffer, eflags, 14, "NT");
        DecodeEflagPart(buffer, eflags, 16, "RF");
        DecodeEflagPart(buffer, eflags, 17, "VM");
        DecodeEflagPart(buffer, eflags, 18, "AC");
        DecodeEflagPart(buffer, eflags, 19, "VIF");
        DecodeEflagPart(buffer, eflags, 20, "VIP");
        DecodeEflagPart(buffer, eflags, 21, "ID");
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
        String::StringBuffer eflagsBuffer { eflags, 33 };
        String::IntegerToString(eflagsBuffer, data->EFLAGS, 2);

        char decodedEflags[43];
        String::StringBuffer decodedEflagsBuffer{ decodedEflags, 43 };
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
                                  data->ESI, data->EDI, data->EIP, eflags, decodedEflags
        );

        if (!formatOk) {
            Bootloader::GetBootloader()->Panic("Interrupt info collection failed");
        }

        Bootloader::GetBootloader()->Panic(panicBuffer.Data);
    }

    void handler(HW::InterruptData* data) {
        FB_LOG_WARNING("Interrupt :woah:");
    }

    void SetupInterrupts() {
        HW::RegisterUnknownInterruptHandler(&UnknownInterruptHandler);
        HW::RegisterInterruptHandler(static_cast<HW::InterruptType>(0x69), handler);
        HW::SetupInterrupts();
    }
}  // namespace FunnyOS::Bootloader32
