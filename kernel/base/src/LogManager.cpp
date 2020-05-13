#include <FunnyOS/Kernel/LogManager.hpp>

namespace FunnyOS::Kernel {

    void LogManager::EnableOnscreenLogging(Stdlib::Ref<Misc::TerminalManager::TerminalManager> manager) {
        m_terminalManagerLoggingSink =
            Stdlib::MakeRef<Misc::TerminalManager::TerminalManagerLoggingSink>(Move(manager));
        m_logger.AddSink(Stdlib::StaticRefCast<Stdlib::ILoggingSink>(m_terminalManagerLoggingSink));
    }

    void LogManager::DisableOnscreenLogging() {
        m_logger.RemoveSink(m_terminalManagerLoggingSink.Get());
    }

    Stdlib::Logger& LogManager::GetLogger() {
        return m_logger;
    }

    LogManager::LogManager() = default;

}  // namespace FunnyOS::Kernel