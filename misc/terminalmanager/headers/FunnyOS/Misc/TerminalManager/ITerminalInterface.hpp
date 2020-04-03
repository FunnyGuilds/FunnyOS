#ifndef FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_ITERMINALINTERFACE_HPP
#define FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_ITERMINALINTERFACE_HPP

#include <FunnyOS/Stdlib/System.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Misc::TerminalManager {
    /**
     * Represents either a background or a foreground color of a character
     */
    enum class Color {
        Black = 0x00,
        Blue = 0x01,
        Green = 0x02,
        Cyan = 0x03,
        Red = 0x04,
        Magenta = 0x05,
        Brown = 0x06,
        LightGray = 0x07,
        DarkGray = 0x08,
        LightBlue = 0x09,
        LightGreen = 0x0A,
        LightCyan = 0x0B,
        LightRed = 0x0C,
        LightMagenta = 0x0D,
        Yellow = 0x0E,
        White = 0x0F,
    };

    /**
     * Represents a position of the cursor on the screen.
     */
    struct CursorPosition {
        uint8_t X;
        uint8_t Y;
    };

    /**
     * Represents data about a character that can be put on the screen.
     */
    struct CharacterData {
        /**
         * The character code
         */
        char Character;

        /**
         * Foreground color
         */
        Color Background;

        /**
         * Background color
         */
        Color Foreground;
    };

    /**
     * Interface for low-level screen interactions
     */
    class ITerminalInterface {
       public:
        /**
         * @return width of the screen
         */
        [[nodiscard]] virtual uint8_t GetScreenWidth() const noexcept = 0;

        /**
         * @return height of the screen
         */
        [[nodiscard]] virtual uint8_t GetScreenHeight() const noexcept = 0;

        /**
         * @return position of the cursor
         */
        [[nodiscard]] virtual CursorPosition GetCursorPosition() const noexcept = 0;

        /**
         * Sets the cursor's position
         *
         * @param position  new position of the cursor
         *
         * @return  true if success, false if position out of boundaries
         */
        virtual bool SetCursorPosition(const CursorPosition& position) noexcept = 0;

        /**
         * Writes a character described by data to the specific screen position.
         *
         * @param[in] position  position where to write it
         * @param[in] data  character data
         */
        virtual void WriteCharacter(const CursorPosition& position, const CharacterData& data) noexcept = 0;

        /**
         * Reads a character data from the specific screen position.
         *
         * @param[in] position  position where to read from
         * @return character data
         */
        virtual CharacterData ReadCharacter(const CursorPosition& position) noexcept = 0;

        /**
         * Copies character data from the [from] position to the [to] position.
         *
         * @param[in] from position to copy from
         * @param[in] to position to copy to
         */
        virtual void Move(const CursorPosition& from, const CursorPosition& to) noexcept = 0;
    };

}  // namespace FunnyOS::Misc::TerminalManager

#endif  // FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_ITERMINALINTERFACE_HPP
