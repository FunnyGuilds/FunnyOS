#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_LOGGING_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_LOGGING_HPP

#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Bootloader::Logging {

    /**
     * Severity level of a log message.
     */
    enum class LogLevel { Debug, Info, Ok, Warning, Error, Fatal };

    /**
     * Gets the TerminalManager instance used by the logging system.
     */
    Misc::TerminalManager::TerminalManager* GetTerminalManager();

    /**
     * Posts a new message to the logging system.
     *
     * @param[in] level severity of the message
     * @param[in] message content of the message
     */
    void PostLog(LogLevel level, const char* message);

}  // namespace FunnyOS::Bootloader::Logging

#define FB_LOG(level, message)                                                                         \
    do {                                                                                               \
        FunnyOS::Bootloader::Logging::PostLog(FunnyOS::Bootloader::Logging::LogLevel::level, message); \
    } while (0)

#define FB_LOG_DEBUG(message) FB_LOG(Debug, message)
#define FB_LOG_INFO(message) FB_LOG(Info, message)
#define FB_LOG_OK(message) FB_LOG(Ok, message)
#define FB_LOG_WARNING(message) FB_LOG(Warning, message)
#define FB_LOG_ERROR(message) FB_LOG(Error, message)
#define FB_LOG_FATAL(message) FB_LOG(Fatal, message)

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_LOGGING_HPP
