#include <FunnyOS/Hardware/PIC.hpp>

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>
#include <FunnyOS/Hardware/InputOutput.hpp>

namespace FunnyOS::HW::PIC {
    using namespace HW::InputOutput;

    namespace {
        constexpr uint16_t PORT_MASTER_PIC_COMMAND = 0x0020;
        constexpr uint16_t PORT_MASTER_PIC_DATA = PORT_MASTER_PIC_COMMAND + 1;
        constexpr uint16_t PORT_SLAVE_PIC_COMMAND = 0x00A0;
        constexpr uint16_t PORT_SLAVE_PIC_DATA = PORT_SLAVE_PIC_COMMAND + 1;
    }  // namespace

    bool IsPICInterrupt(InterruptType type) {
        return type >= InterruptType::IRQ_PIT_Interrupt && type <= InterruptType::IRQ_SecondaryAtaHardDriveInterrupt;
    }

    bool IsMasterPICInterrupt(InterruptType type) {
        return type >= InterruptType::IRQ_PIT_Interrupt && type <= InterruptType::IRQ_LPT1_Interrupt;
    }

    namespace {
        void PicRemapRoutine() {
            const uint8_t icw1 = 1 << 0 |  // ICW4 needed
                                 1 << 4;   // Required ICW1 bit

            // ICW1
            OutputByte(PORT_MASTER_PIC_COMMAND, icw1);
            IOWait();
            OutputByte(PORT_SLAVE_PIC_COMMAND, icw1);
            IOWait();

            // ICW2
            OutputByte(PORT_MASTER_PIC_DATA, static_cast<uint8_t>(InterruptType::IRQ_PIT_Interrupt));
            IOWait();
            OutputByte(PORT_SLAVE_PIC_DATA, static_cast<uint8_t>(InterruptType::IRQ_CMOS_RealTimeClockInterrupt));
            IOWait();

            // ICW3
            OutputByte(PORT_MASTER_PIC_DATA, 1 << 2);  // Slave is at IRQ2
            IOWait();
            OutputByte(PORT_SLAVE_PIC_DATA, 2);
            IOWait();

            // ICW4
            const uint8_t icw4 = 1 << 0;  // 8086 mode
            OutputByte(PORT_MASTER_PIC_DATA, icw4);
            IOWait();
            OutputByte(PORT_SLAVE_PIC_DATA, icw4);
            IOWait();
        }
    }  // namespace

    void Remap() {
        HW::NoInterruptsBlock noInterrupts;
        const uint8_t masterMask = InputByte(PORT_MASTER_PIC_DATA);
        const uint8_t slaveMask = InputByte(PORT_SLAVE_PIC_DATA);

        PicRemapRoutine();

        OutputByte(PORT_MASTER_PIC_DATA, masterMask);
        OutputByte(PORT_SLAVE_PIC_DATA, slaveMask);
    }

    void SendEndOfInterrupt(bool master) {
        const size_t ocw2 = 1 << 5; // End of interrupt
        OutputByte(master ? PORT_MASTER_PIC_COMMAND : PORT_SLAVE_PIC_COMMAND, ocw2);
    }
}  // namespace FunnyOS::HW::PIC
