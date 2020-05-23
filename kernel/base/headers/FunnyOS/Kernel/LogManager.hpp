#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_LOGMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_LOGMANAGER_HPP

#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManagerLoggingSink.hpp>

namespace FunnyOS::Kernel {

    class LogManager {
       public:
        void EnableOnscreenLogging(Stdlib::Ref<Misc::TerminalManager::TerminalManager> manager);

        void DisableOnscreenLogging();

        Stdlib::Logger& GetLogger();

       private:
        LogManager();

        friend class Kernel64;

       private:
        Stdlib::Logger m_logger{};
        Stdlib::Ref<Misc::TerminalManager::TerminalManagerLoggingSink> m_terminalManagerLoggingSink{};
    };

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_LOGMANAGER_HPP
