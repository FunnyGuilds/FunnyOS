#include <FunnyOS/Hardware/Interrupts.hpp>

#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Hardware/PIC.hpp>
#include "InterruptSetup/InterruptSetup.hpp"

using namespace FunnyOS::Stdlib;

namespace FunnyOS::HW {
    Stdlib::Optional<const char*> GetInterruptMnemonic(InterruptType type) {
        switch (type) {
            case InterruptType::DivideError:
                return Stdlib::MakeOptional<const char*>("#DE");
            case InterruptType::DebugException:
                return Stdlib::MakeOptional<const char*>("#DB");
            case InterruptType::Breakpoint:
                return Stdlib::MakeOptional<const char*>("#BP");
            case InterruptType::Overflow:
                return Stdlib::MakeOptional<const char*>("#OF");
            case InterruptType::BoundRangeExceeded:
                return Stdlib::MakeOptional<const char*>("#BR");
            case InterruptType::InvalidOpcode:
                return Stdlib::MakeOptional<const char*>("#UD");
            case InterruptType::DeviceNotAvailable:
                return Stdlib::MakeOptional<const char*>("#NM");
            case InterruptType::DoubleFault:
                return Stdlib::MakeOptional<const char*>("#DF");
            case InterruptType::InvalidTSS:
                return Stdlib::MakeOptional<const char*>("#TS");
            case InterruptType::SegmentNotPresent:
                return Stdlib::MakeOptional<const char*>("#NP");
            case InterruptType::StackSegmentFault:
                return Stdlib::MakeOptional<const char*>("#SS");
            case InterruptType::GeneralProtection:
                return Stdlib::MakeOptional<const char*>("#GP");
            case InterruptType::PageFault:
                return Stdlib::MakeOptional<const char*>("#PF");
            case InterruptType::x87FPU_FloatingPointError:
                return Stdlib::MakeOptional<const char*>("#MF");
            case InterruptType::AlignmentCheck:
                return Stdlib::MakeOptional<const char*>("#AC");
            case InterruptType::MachineCheck:
                return Stdlib::MakeOptional<const char*>("#MC");
            case InterruptType::SIMD_FloatingPointException:
                return Stdlib::MakeOptional<const char*>("#XM");
            case InterruptType::VirtualizationException:
                return Stdlib::MakeOptional<const char*>("#VE");
            default:
                break;
        }

        return Stdlib::EmptyOptional<const char*>();
    }

    namespace {
        InterruptHandler g_interruptHandlers[INTERRUPTS_COUNT];
        InterruptHandler g_unknownInterruptHandler;

        void InterruptHandlerSelector(InterruptData* data) {
            const InterruptType type = data->Type;
            InterruptHandler handler = g_interruptHandlers[static_cast<int>(data->Type)];

            if (handler == nullptr) {
                handler = g_unknownInterruptHandler;
            }

            handler(data);

            // Send PIC End of interrupt command.
            if (PIC::IsPICInterrupt(type)) {
                if (!PIC::IsMasterPICInterrupt(type)) {
                    PIC::SendEndOfInterrupt(false);
                }

                PIC::SendEndOfInterrupt(true);
            }
        }
    }  // namespace

    void RegisterInterruptHandler(InterruptType type, InterruptHandler handler) {
        g_interruptHandlers[static_cast<int>(type)] = handler;
    }

    void UnregisterInterruptHandler(InterruptType type) {
        RegisterInterruptHandler(type, nullptr);
    }

    void RegisterUnknownInterruptHandler(InterruptHandler handler) {
        g_unknownInterruptHandler = handler;
    }

    void SetupInterrupts() {
        InterruptSetup::SetupInterruptTable(&InterruptHandlerSelector);
    }

    void EnableHardwareInterrupts() {
#ifdef __GNUC__
        asm volatile("sti");
#endif
    }

    void DisableHardwareInterrupts() {
#ifdef __GNUC__
        asm volatile("cli");
#endif
    }

    bool HardwareInterruptsEnabled() {
        return (CPU::GetFlagsRegister() & CPU::Flags::InterruptFlag) != 0;
    }

}  // namespace FunnyOS::HW