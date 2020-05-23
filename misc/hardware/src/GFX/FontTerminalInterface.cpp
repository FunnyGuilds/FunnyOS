#include <FunnyOS/Hardware/GFX/FontTerminalInterface.hpp>

#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>

namespace FunnyOS::HW {
    namespace {
        constexpr const uint8_t FONT_WIDTH  = 8;
        constexpr const uint8_t FONT_HEIGHT = 16;

        constexpr const uint8_t g_colorMap[16][3] = {
            {0x00, 0x00, 0x00}, {0x00, 0x00, 0xAA}, {0x00, 0xAA, 0x00}, {0x00, 0xAA, 0xAA},
            {0xAA, 0x00, 0x00}, {0xAA, 0x00, 0xAA}, {0xAA, 0x55, 0x00}, {0xAA, 0xAA, 0xAA},
            {0x55, 0x55, 0x55}, {0x55, 0x55, 0xFF}, {0x55, 0xFF, 0x55}, {0x55, 0xFF, 0xFF},
            {0xFF, 0x55, 0x55}, {0xFF, 0x55, 0xFF}, {0xFF, 0xFF, 0x55}, {0xFF, 0xFF, 0xFF},
        };
    }  // namespace

    FontTerminalInterface::FontTerminalInterface(uint8_t* fontLocation, Stdlib::Ref<FramebufferInterface> interface)
        : m_fontLocation(fontLocation),
          m_interface(Stdlib::Move(interface)),
          m_characterData(
              (m_interface->GetScreenWidth() / FONT_WIDTH) * (m_interface->GetScreenHeight() / FONT_HEIGHT)) {
        const CharacterDataCache defaultData = {
            {0, Misc::TerminalManager::Color::Black, Misc::TerminalManager::Color::White},
            false};
        Stdlib::Memory::Set(m_characterData, defaultData);
    }

    Stdlib::Memory::SizedBuffer<uint8_t> FontTerminalInterface::SaveScreenData() const noexcept {
        auto data = Stdlib::Memory::AllocateBuffer<uint8_t>(sizeof(CharacterData) * m_characterData.Size);
        Stdlib::Memory::Copy(data.Data, m_characterData.Data, data.Size);
        return data;
    }

    void FontTerminalInterface::RestoreScreenData(Stdlib::Memory::SizedBuffer<uint8_t>& buffer) noexcept {
        Stdlib::Memory::Copy(m_characterData.Data, buffer.Data, buffer.Size);
        Stdlib::Memory::FreeBuffer(m_characterData);

        for (auto& data : m_characterData) {
            data.Dirty = true;
        }
    }

    uint16_t FontTerminalInterface::GetScreenWidth() const noexcept {
        return m_interface->GetScreenWidth() / FONT_WIDTH;
    }

    uint16_t FontTerminalInterface::GetScreenHeight() const noexcept {
        return m_interface->GetScreenHeight() / FONT_HEIGHT;
    }

    FontTerminalInterface::CursorPosition FontTerminalInterface::GetCursorPosition() const noexcept {
        return m_cursorPosition;
    }

    bool FontTerminalInterface::SetCursorPosition(const CursorPosition& position) noexcept {
        m_cursorPosition = position;
        return true;
    }

    void FontTerminalInterface::WriteCharacter(const CursorPosition& position, const CharacterData& data) noexcept {
        *m_characterData[CharacterIndexAt(position)] = {data, true};
    }

    FontTerminalInterface::CharacterData FontTerminalInterface::ReadCharacter(const CursorPosition& position) noexcept {
        return m_characterData[CharacterIndexAt(position)]->Data;
    }

    void FontTerminalInterface::Move(const CursorPosition& from, const CursorPosition& to) noexcept {
        auto& source = *m_characterData[CharacterIndexAt(from)];
        source.Dirty = true;

        *m_characterData[CharacterIndexAt(to)] = source;
    }

    uint32_t FontTerminalInterface::CharacterIndexAt(CursorPosition position) const noexcept {
        return position.Y * GetScreenWidth() + position.X;
    }

    void FontTerminalInterface::Submit() {
        for (uint16_t x = 0; x < GetScreenWidth(); x++) {
            for (uint16_t y = 0; y < GetScreenHeight(); y++) {
                CharacterDataCache& data = *m_characterData[CharacterIndexAt({x, y})];
                if (!data.Dirty) {
                    continue;
                }

                DrawSingle({x, y}, data);
            }
        }
    }

    void FontTerminalInterface::DrawSingle(CursorPosition position, CharacterDataCache& data) {
        uint8_t* fontCharacter = m_fontLocation + (static_cast<uint16_t>(data.Data.Character) * 16);

        for (size_t x = 0; x < FONT_WIDTH; x++) {
            for (size_t y = 0; y < FONT_HEIGHT; y++) {
                const bool isForeground = (fontCharacter[y] & (1 << (7 - x))) != 0;
                const uint8_t* c        = g_colorMap
                    [isForeground ? static_cast<size_t>(data.Data.Foreground)
                                  : static_cast<size_t>(data.Data.Background)];
                m_interface->PutPixel(position.X * FONT_WIDTH + x, position.Y * FONT_HEIGHT + y, c[0], c[1], c[2]);
            }
        }

        data.Dirty = false;
    }

}  // namespace FunnyOS::HW