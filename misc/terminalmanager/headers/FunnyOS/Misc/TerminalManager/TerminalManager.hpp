#ifndef FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGER_HPP
#define FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGER_HPP

#include "ITerminalInterface.hpp"

namespace FunnyOS::Misc::TerminalManager {

    /**
     * Exception thrown by the TerminalManager when something fails.
     */
    class TerminalManagerException : public Stdlib::System::Exception {
       public:
        explicit TerminalManagerException(const char* message);

        [[nodiscard]] const char* GetMessage() const noexcept override;

       private:
        const char* m_message;
    };

    class TerminalManager {
       public:
        /**
         * Creates a new TerminalManager using a ITerminalInterface as backend.
         *
         * @param[in] interface interface to be used as a backend
         */
        explicit TerminalManager(ITerminalInterface* interface);

        /**
         * @return the ITerminalInterface this manager uses.
         */
        [[nodiscard]] ITerminalInterface* GetInterface() const noexcept;

        /**
         * @return the current background color
         */
        [[nodiscard]] Color GetBackgroundColor() const noexcept;

        /**
         * @return the current foreground color
         */
        [[nodiscard]] Color GetForegroundColor() const noexcept;

        /**
         * Changes both the background and the foreground color
         *
         * @param background new background color
         * @param foreground new foreground color
         */
        void ChangeColor(Color background, Color foreground) noexcept;

        /**
         * Changes the background color
         *
         * @param background new background color
         */
        void ChangeBackgroundColor(Color background) noexcept;

        /**
         * Changes the foreground color
         *
         * @param background new background color
         */
        void ChangeForegroundColor(Color foreground) noexcept;

        /**
         * Prints a singular character to the output.
         *
         * @param character character to print.
         */
        void PrintCharacter(char character);

        /**
         * Prints a string to the output.
         *
         * @param string string to print.
         */
        void PrintString(const char* string);

        /**
         * Prints a new line.
         */
        void PrintLine();

        /**
         * Prints a string followed by
         * @param line string to print
         */
        void PrintLine(const char* line);

        /**
         * Scrolls one terminal line.
         *
         * Equivalent of calling ScrollLines(1)
         */
        void ScrollLine();

        /**
         * Scrolls terminal downwards by N lines.
         *
         * @param n number of lines to scroll.
         */
        void ScrollLines(uint16_t n);

        /**
         * Clears the entire terminal's Nth line using the background color.
         *
         * @param n line to clear
         */
        void ClearLine(uint16_t n);

        /**
         * Clears the entire current line using the background color.
         */
        void ClearLine();

        /**
         * Clears the entire screen using the background color.
         */
        void ClearScreen();

       private:
        void BeginUpdateSession() noexcept;

        bool FinishUpdateSession();

        CursorPosition& SessionCursor();

        void HandleEscapeCode(char character);

        void ScrollIfNecessary();

       private:
        int m_sessionNestingLevel = 0;
        CursorPosition m_sessionCursor{};
        ITerminalInterface* m_interface;
        Color m_backgroundColor = Color::Black;
        Color m_foregroundColor = Color::White;
    };

}  // namespace FunnyOS::Misc::TerminalManager

#endif  // FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGER_HPP
