#include <FunnyOS/Hardware/GFX/VGA.hpp>

#include <FunnyOS/Hardware/InputOutput.hpp>

namespace FunnyOS::HW {
    using namespace FunnyOS::Misc::TerminalManager;
    using namespace FunnyOS::Stdlib;

    namespace {
        /** Base of the VGA video memory */
        auto* VIDEO_MEMORY_BASE = reinterpret_cast<uint8_t*>(0xB8000);

        constexpr const uint16_t VGA_PORT_REGISTER_1                     = 0x03D4;
        constexpr const uint8_t VGA_PORT_REGISTER_1_CURSOR_POSITION_HIGH = 0x0E;
        constexpr const uint8_t VGA_PORT_REGISTER_1_CURSOR_POSITION_LOW  = 0x0F;

        /** Read a value from VGA register [registerAddress] at index [index] */
        uint8_t ReadRegister(uint16_t registerAddress, uint8_t index) {
            InputOutput::OutputByte(registerAddress, index);
            return InputOutput::InputByte(static_cast<uint16_t>(registerAddress + 1));
        }

        /** Write a [value] to a VGA register [registerAddress] at index [index] */
        void WriteRegister(uint16_t address, uint8_t index, uint8_t value) {
            InputOutput::OutputByte(address, index);
            InputOutput::OutputByte(static_cast<uint16_t>(address + 1), value);
        }

        /** Convert CursorPosition to VGA position index. */
        uint16_t ToVGAPosition(const CursorPosition& position, uint8_t screenWidth) {
            return position.Y * screenWidth + position.X;
        }

        /** Convert VGA position index to CursorPosition */
        CursorPosition FromVGAPosition(uint16_t position, uint8_t screenWidth) {
            return CursorPosition{.X = static_cast<uint8_t>(position % screenWidth),
                                  .Y = static_cast<uint8_t>(position / screenWidth)};
        }
    }  // namespace

    // Screen dimensions
    constexpr const uint16_t SCREEN_WIDTH  = 80;
    constexpr const uint16_t SCREEN_HEIGHT = 25;

    Memory::SizedBuffer<uint8_t> VGAInterface::SaveScreenData() const noexcept {
        const size_t videoMemorySize = GetScreenWidth() * GetScreenHeight() * 2U;
        const size_t bufferSize      = videoMemorySize + sizeof(CursorPosition);

        const CursorPosition cursorPosition = GetCursorPosition();

        Memory::SizedBuffer<uint8_t> buffer = Memory::AllocateBuffer<uint8_t>(bufferSize);
        Memory::Copy(buffer.Data, VIDEO_MEMORY_BASE, videoMemorySize);
        Memory::Copy(buffer.Data + videoMemorySize, &cursorPosition, sizeof(CursorPosition));

        return buffer;
    }

    void VGAInterface::RestoreScreenData(Memory::SizedBuffer<uint8_t>& buffer) noexcept {
        const size_t videoMemorySize = GetScreenWidth() * GetScreenHeight() * 2U;

        CursorPosition cursorPosition;

        Memory::Copy(VIDEO_MEMORY_BASE, buffer.Data, videoMemorySize);
        Memory::Copy(&cursorPosition, buffer.Data + videoMemorySize, sizeof(CursorPosition));
        SetCursorPosition(cursorPosition);

        Memory::FreeBuffer(buffer);
    }

    uint16_t VGAInterface::GetScreenWidth() const noexcept {
        return SCREEN_WIDTH;
    }

    uint16_t VGAInterface::GetScreenHeight() const noexcept {
        return SCREEN_HEIGHT;
    }

    CursorPosition VGAInterface::GetCursorPosition() const noexcept {
        const uint16_t vgaPosition = ReadRegister(VGA_PORT_REGISTER_1, VGA_PORT_REGISTER_1_CURSOR_POSITION_LOW) |
                                     ReadRegister(VGA_PORT_REGISTER_1, VGA_PORT_REGISTER_1_CURSOR_POSITION_HIGH) << 8U;

        return FromVGAPosition(vgaPosition, GetScreenWidth());
    }

    bool VGAInterface::SetCursorPosition(const CursorPosition& position) noexcept {
        if (position.X >= GetScreenWidth() || position.Y >= GetScreenHeight()) {
            return false;
        }

        const uint16_t vgaPosition = ToVGAPosition(position, GetScreenWidth());

        WriteRegister(
            VGA_PORT_REGISTER_1, VGA_PORT_REGISTER_1_CURSOR_POSITION_LOW, static_cast<uint8_t>(vgaPosition & 0xFFU));
        WriteRegister(
            VGA_PORT_REGISTER_1, VGA_PORT_REGISTER_1_CURSOR_POSITION_HIGH,
            static_cast<uint8_t>((vgaPosition >> 8) & 0xFF));
        return true;
    }

    void VGAInterface::WriteCharacter(const CursorPosition& position, const CharacterData& data) noexcept {
        const uint16_t vgaPosition = ToVGAPosition(position, GetScreenWidth());
        const uint8_t color        = static_cast<uint8_t>(data.Background) << 4 | static_cast<uint8_t>(data.Foreground);

        *(VIDEO_MEMORY_BASE + vgaPosition * 2 + 0x00) = static_cast<uint8_t>(data.Character);
        *(VIDEO_MEMORY_BASE + vgaPosition * 2 + 0x01) = color;
    }

    CharacterData VGAInterface::ReadCharacter(const CursorPosition& position) noexcept {
        const uint16_t vgaPosition = ToVGAPosition(position, GetScreenWidth());

        const char character = *(VIDEO_MEMORY_BASE + vgaPosition * 2 + 0x00);
        const uint8_t color  = *(VIDEO_MEMORY_BASE + vgaPosition * 2 + 0x01);

        return CharacterData{
            .Character  = character,
            .Background = static_cast<Color>(color >> 4),
            .Foreground = static_cast<Color>(color & 0xF),
        };
    }

    void VGAInterface::Move(const CursorPosition& from, const CursorPosition& to) noexcept {
        const uint16_t fromPosition = ToVGAPosition(from, GetScreenWidth());
        const uint16_t toPosition   = ToVGAPosition(to, GetScreenWidth());

        *(VIDEO_MEMORY_BASE + toPosition * 2 + 0x00) = *(VIDEO_MEMORY_BASE + fromPosition * 2 + 0x00);
        *(VIDEO_MEMORY_BASE + toPosition * 2 + 0x01) = *(VIDEO_MEMORY_BASE + fromPosition * 2 + 0x01);
    }

    void VGAInterface::Submit() {}

}  // namespace FunnyOS::HW