#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_VGAINTERFACE_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_VGAINTERFACE_HPP

#include <FunnyOS/Misc/TerminalManager/ITerminalInterface.hpp>

namespace FunnyOS::HW {

    /**
     * Implementation of ITerminalInterface that operates directly on the VGA hardware.
     */
    class VGAInterface : public Misc::TerminalManager::ITerminalInterface {
       public:
        using CursorPosition = Misc::TerminalManager::CursorPosition;
        using CharacterData = Misc::TerminalManager::CharacterData;

       public:
        [[nodiscard]] uint8_t GetScreenWidth() const noexcept override;

        [[nodiscard]] uint8_t GetScreenHeight() const noexcept override;

        [[nodiscard]] CursorPosition GetCursorPosition() const noexcept override;

        bool SetCursorPosition(const CursorPosition& position) noexcept override;

        void WriteCharacter(const CursorPosition& position, const CharacterData& data) noexcept override;

        [[nodiscard]] CharacterData ReadCharacter(const CursorPosition& position) noexcept override;

        void Move(const CursorPosition& from, const CursorPosition& to) noexcept override;
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_VGAINTERFACE_HPP
