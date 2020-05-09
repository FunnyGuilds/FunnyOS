#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>

#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Misc::TerminalManager {
    TerminalManagerLoggingSink::TerminalManagerLoggingSink(Stdlib::Ref<TerminalManager> manager)
        : m_terminalManager(Move(manager)) {}

    void TerminalManagerLoggingSink::SubmitMessage(Stdlib::LogLevel level, const char* message) {
        static constexpr const Color c_logLevelColors[] = {Color::Cyan,   Color::LightBlue, Color::LightGreen,
                                                           Color::Yellow, Color::LightRed,  Color::Red};

        const Color preservedColor = m_terminalManager->GetForegroundColor();

        // Tag start
        m_terminalManager->ChangeForegroundColor(Color::White);
        m_terminalManager->PrintString(" * [");

        // Actual tag
        m_terminalManager->ChangeForegroundColor(c_logLevelColors[static_cast<int>(level)]);
        m_terminalManager->PrintString(Stdlib::GetLogLevelName(level));

        // Tag finish
        m_terminalManager->ChangeForegroundColor(Color::White);
        m_terminalManager->PrintString("] * ");

        // Message
        m_terminalManager->ChangeForegroundColor(preservedColor);
        m_terminalManager->PrintString(message);
        m_terminalManager->PrintLine();
    }
}  // namespace FunnyOS::Misc::TerminalManager