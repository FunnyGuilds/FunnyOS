#include <FunnyOS/Hardware/Interrupts.hpp>

#include <FunnyOS/Hardware/PIC.hpp>

using namespace FunnyOS::Stdlib;

namespace FunnyOS::HW {

    constexpr const char* g_interruptMnemonics[] = {
        [static_cast<unsigned int>(InterruptType::DivideError)]                 = "#DE",
        [static_cast<unsigned int>(InterruptType::DebugException)]              = "#DB",
        [static_cast<unsigned int>(InterruptType::NmiInterrupt)]                = nullptr,
        [static_cast<unsigned int>(InterruptType::Breakpoint)]                  = "#BP",
        [static_cast<unsigned int>(InterruptType::Overflow)]                    = "#OF",
        [static_cast<unsigned int>(InterruptType::BoundRangeExceeded)]          = "#BR",
        [static_cast<unsigned int>(InterruptType::InvalidOpcode)]               = "#UD",
        [static_cast<unsigned int>(InterruptType::DeviceNotAvailable)]          = "#NM",
        [static_cast<unsigned int>(InterruptType::DoubleFault)]                 = "#DF",
        [static_cast<unsigned int>(InterruptType::CoprocessorSegmentOverrun)]   = nullptr,
        [static_cast<unsigned int>(InterruptType::InvalidTSS)]                  = "#TS",
        [static_cast<unsigned int>(InterruptType::SegmentNotPresent)]           = "#NP",
        [static_cast<unsigned int>(InterruptType::StackSegmentFault)]           = "#SS",
        [static_cast<unsigned int>(InterruptType::GeneralProtection)]           = "#GP",
        [static_cast<unsigned int>(InterruptType::PageFault)]                   = "#PF",
        [static_cast<unsigned int>(InterruptType::Reserved15)]                  = nullptr,
        [static_cast<unsigned int>(InterruptType::x87FPU_FloatingPointError)]   = "#MF",
        [static_cast<unsigned int>(InterruptType::AlignmentCheck)]              = "#AC",
        [static_cast<unsigned int>(InterruptType::MachineCheck)]                = "#MC",
        [static_cast<unsigned int>(InterruptType::SIMD_FloatingPointException)] = "#XM",
        [static_cast<unsigned int>(InterruptType::VirtualizationException)]     = "#VE",
    };

    Stdlib::Optional<const char*> GetInterruptMnemonic(InterruptType type) {
        const auto index = static_cast<unsigned int>(type);
        if (index >= F_SIZEOF_BUFFER(g_interruptMnemonics)) {
            return Stdlib::EmptyOptional<const char*>();
        }

        const char* mnemonic = g_interruptMnemonics[index];
        return mnemonic == nullptr ? Stdlib::EmptyOptional<const char*>() : Stdlib::MakeOptional<const char*>(mnemonic);
    }

    NoInterruptsBlock::NoInterruptsBlock()
        : m_hadInterrupts(HardwareInterruptsEnabled()),
          m_hadNMIs(NonMaskableInterruptsEnabled()),
          m_picMask(PIC::GetEnabledInterrupts()) {
        if (m_hadInterrupts) {
            DisableHardwareInterrupts();
        }
        if (m_hadNMIs) {
            DisableNonMaskableInterrupts();
        }
        if (m_picMask != 0) {
            PIC::SetEnabledInterrupts(0);
        }
    }

    NoInterruptsBlock::~NoInterruptsBlock() {
        if (m_hadNMIs) {
            EnableNonMaskableInterrupts();
        }
        if (m_hadInterrupts) {
            EnableHardwareInterrupts();
        }
        if (m_picMask != 0) {
            PIC::SetEnabledInterrupts(m_picMask);
        }
    }

}  // namespace FunnyOS::HW