#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Misc::TerminalManager {

    TerminalManager::TerminalManager(ITerminalInterface* interface) : m_interface(interface) {}

    ITerminalInterface* TerminalManager::GetInterface() const noexcept {
        return m_interface;
    }

    Color TerminalManager::GetBackgroundColor() const noexcept {
        return m_backgroundColor;
    }

    Color TerminalManager::GetForegroundColor() const noexcept {
        return m_foregroundColor;
    }

    void TerminalManager::ChangeColor(Color background, Color foreground) noexcept {
        m_backgroundColor = background;
        m_foregroundColor = foreground;
    }

    void TerminalManager::ChangeBackgroundColor(Color background) noexcept {
        ChangeColor(background, GetForegroundColor());
    }

    void TerminalManager::ChangeForegroundColor(Color foreground) noexcept {
        ChangeColor(GetBackgroundColor(), foreground);
    }

    void TerminalManager::PrintCharacter(char character) {
        BeginUpdateSession();

        if (character < 0x20) {
            HandleEscapeCode(character);
        } else {
            m_interface->WriteCharacter(SessionCursor(), {character, m_backgroundColor, m_foregroundColor});
            SessionCursor().X++;
        }

        ScrollIfNecessary();
        FinishUpdateSession();
    }

    void TerminalManager::PrintString(const char* string) {
        BeginUpdateSession();

        for (size_t i = 0; *(string + i) != 0; i++) {
            PrintCharacter(*(string + i));
        }

        FinishUpdateSession();
    }

    void TerminalManager::PrintLine() {
        PrintLine("");
    }

    void TerminalManager::PrintLine(const char* line) {
        BeginUpdateSession();
        PrintString(line);
        PrintCharacter('\r');
        PrintCharacter('\n');
        FinishUpdateSession();
    }

    void TerminalManager::ScrollLine() {
        ScrollLines(1);
    }

    void TerminalManager::ScrollLines(uint8_t n) {
        if (n == 0) {
            return;
        }

        BeginUpdateSession();

        if (n >= m_interface->GetScreenHeight()) {
            ClearScreen();
            SessionCursor().Y = static_cast<uint8_t>(m_interface->GetScreenHeight() - 1);
            FinishUpdateSession();
            return;
        }

        for (uint8_t fromLine = n; fromLine < m_interface->GetScreenHeight(); fromLine++) {
            const int16_t toLine = fromLine - n;

            CursorPosition from{0, static_cast<uint8_t>(fromLine)};
            CursorPosition to{0, static_cast<uint8_t>(toLine)};

            for (uint8_t x = 0; x < m_interface->GetScreenWidth(); x++) {
                from.X = to.X = x;
                m_interface->Move(from, to);
            }
        }

        ClearLine(static_cast<uint8_t>(m_interface->GetScreenHeight() - 1));

        SessionCursor().Y -= n;

        FinishUpdateSession();
    }

    void TerminalManager::ClearLine(uint8_t n) {
        BeginUpdateSession();

        CursorPosition& cursor = SessionCursor();
        cursor.X = 0;
        cursor.Y = n;

        while (cursor.X < m_interface->GetScreenWidth()) {
            m_interface->WriteCharacter(cursor, {' ', m_backgroundColor, m_foregroundColor});
            cursor.X++;
        }

        cursor.X = 0;
        cursor.Y++;

        FinishUpdateSession();
    }

    void TerminalManager::ClearLine() {
        BeginUpdateSession();
        ClearLine(SessionCursor().Y);
        FinishUpdateSession();
    }

    void TerminalManager::ClearScreen() {
        BeginUpdateSession();

        for (uint8_t i = 0; i < m_interface->GetScreenHeight(); i++) {
            ClearLine(i);
        }

        SessionCursor().X = 0;
        SessionCursor().Y = 0;

        FinishUpdateSession();
    }

    void TerminalManager::BeginUpdateSession() noexcept {
        if (m_sessionNestingLevel == 0) {
            m_sessionCursor = m_interface->GetCursorPosition();
        }

        m_sessionNestingLevel++;
    }

    bool TerminalManager::FinishUpdateSession() {
        if (--m_sessionNestingLevel == 0) {
            if (!m_interface->SetCursorPosition(m_sessionCursor)) {
                m_sessionCursor.X = 0;
                m_sessionCursor.Y = 0;
                m_interface->SetCursorPosition(m_sessionCursor);
            }

            return true;
        }

        if (m_sessionNestingLevel < 0) {
            throw TerminalManagerException("sessionNestingLevel < 0");
        }

        return false;
    }

    CursorPosition& TerminalManager::SessionCursor() {
        if (m_sessionNestingLevel == 0) {
            throw TerminalManagerException("sessionNestingLevel == 0");
        }
        return m_sessionCursor;
    }

    void TerminalManager::HandleEscapeCode(char character) noexcept {
        CursorPosition& cursor = SessionCursor();

        switch (character) {
            case '\n':
                cursor.Y++;
                break;
            case '\r':
                cursor.X = 0;
                break;
            case '\t':
                // TODO: customizable tab size maybe
                cursor.X += 4 - (cursor.X % 4);
                break;
            default:
                // not supported
                break;
        }
    }

    void TerminalManager::ScrollIfNecessary() noexcept {
        CursorPosition& cursor = SessionCursor();

        if (cursor.X >= m_interface->GetScreenWidth()) {
            cursor.X = 0;
            cursor.Y++;
        }

        const auto scrollAmount = static_cast<int16_t>(
            static_cast<int16_t>(cursor.Y) - static_cast<int16_t>(m_interface->GetScreenHeight()) + 1);

        if (scrollAmount <= 0) {
            return;
        }

        ScrollLines(static_cast<uint8_t>(scrollAmount));
    }

    TerminalManagerException::TerminalManagerException(const char* message) : m_message(message) {}

    const char* TerminalManagerException::GetMessage() const noexcept {
        return m_message;
    }
}  // namespace FunnyOS::Misc::TerminalManager