#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FONTTERMINALINTERFACE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FONTTERMINALINTERFACE_HPP

#include <FunnyOS/Stdlib/Dynamic.hpp>
#include <FunnyOS/Misc/TerminalManager/ITerminalInterface.hpp>

namespace FunnyOS::HW {

    class FramebufferInterface;  // FramebufferInterface.hpp

    /**
     * A ITerminalInterface that used a 8x16 fonts to print characters on a raw frame buffer.
     */
    class FontTerminalInterface : public Misc::TerminalManager::ITerminalInterface {
       public:
        using CursorPosition = Misc::TerminalManager::CursorPosition;
        using CharacterData  = Misc::TerminalManager::CharacterData;

        /**
         * Internal structure used to store character data.
         */
        struct CharacterDataCache {
            /**
             * Information about the character itself.
             */
            CharacterData Data;

            /**
             * Whether or not this data changed since it was rendered last time.
             */
            bool Dirty;
        };

       public:
        /**
         * Constructs a new FontTerminalInterface.
         *
         * @param fontLocation pointer to 8x16 font to be used.
         * @param interface FramebufferInterface to draw on.
         */
        FontTerminalInterface(uint8_t* fontLocation, Stdlib::Ref<FramebufferInterface> interface);

       public:
        [[nodiscard]] Stdlib::Memory::SizedBuffer<uint8_t> SaveScreenData() const noexcept override;

        void RestoreScreenData(Stdlib::Memory::SizedBuffer<uint8_t>& buffer) noexcept override;

        [[nodiscard]] uint16_t GetScreenWidth() const noexcept override;

        [[nodiscard]] uint16_t GetScreenHeight() const noexcept override;

        [[nodiscard]] CursorPosition GetCursorPosition() const noexcept override;

        bool SetCursorPosition(const CursorPosition& position) noexcept override;

        void WriteCharacter(const CursorPosition& position, const CharacterData& data) noexcept override;

        [[nodiscard]] CharacterData ReadCharacter(const CursorPosition& position) noexcept override;

        void Move(const CursorPosition& from, const CursorPosition& to) noexcept override;

        void Submit() override;

       private:
        [[nodiscard]] uint32_t CharacterIndexAt(CursorPosition position) const noexcept;

        void DrawSingle(CursorPosition position, CharacterDataCache& data);

       private:
        uint8_t* m_fontLocation;
        Stdlib::Ref<FramebufferInterface> m_interface;
        Stdlib::SmartSizedBuffer<CharacterDataCache> m_characterData;
        CursorPosition m_cursorPosition = {0, 0};
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FONTTERMINALINTERFACE_HPP
