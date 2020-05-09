#ifndef FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGERLOGGINGSINK_HPP
#define FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGERLOGGINGSINK_HPP

#include <FunnyOS/Stdlib/Logging.hpp>

namespace FunnyOS::Misc::TerminalManager {
    class TerminalManager;  // TerminalManager.hpp

    /**
     * An ILoggingSink implementation that outputs to a TerminalManager.
     */
    class TerminalManagerLoggingSink : public Stdlib::ILoggingSink {
       public:
        /**
         * Constructs a new TerminalManagerLoggingSink.
         *
         * @param terminalManager terminal manager to output the logs to.
         */
        TerminalManagerLoggingSink(Stdlib::Ref<TerminalManager> terminalManager);

       public:
        void SubmitMessage(Stdlib::LogLevel level, const char* message) override;

       private:
        Stdlib::Ref<TerminalManager> m_terminalManager;
    };

}  // namespace FunnyOS::Misc::TerminalManager

#endif  // FUNNYOS_MISC_TERMINALMANAGER_HEADERS_FUNNYOS_MISC_TERMINALMANAGER_TERMINALMANAGERLOGGINGSINK_HPP
