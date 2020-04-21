#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Bootloader32::Logging {

    /**
     * Severity level of a log message.
     */
    enum class LogLevel { Debug, Info, Ok, Warning, Error, Fatal };

    /**
     * Returns whether or not logging debug mode is enabled. (Debug messages are actually printed on the screen)
     * @return whether or not logging debug mode is enabled.
     */
    [[nodiscard]] bool IsDebugModeEnabled();

    /**
     * Sets whether or not logging debug mode is enabled.
     *
     * @param enabled whether or not to enable it
     */
    void SetDebugModeEnabled(bool enabled);

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

    /**
     * Posts a new message to the logging system formatted via String::Format
     *
     * @param[in] level severity of the message
     * @param[in] format message format
     * @param ... String::Format parameters.
     */
    void PostLogFormatted(LogLevel level, const char* format, ...);

}  // namespace FunnyOS::Bootloader32::Logging

#define FB_LOG(level, message)                                                                             \
    do {                                                                                                   \
        FunnyOS::Bootloader32::Logging::PostLog(FunnyOS::Bootloader32::Logging::LogLevel::level, message); \
    } while (0)

#define FB_LOG_F(level, message, ...)                                                                              \
    do {                                                                                                           \
        FunnyOS::Bootloader32::Logging::PostLogFormatted(FunnyOS::Bootloader32::Logging::LogLevel::level, message, \
                                                         __VA_ARGS__);                                             \
    } while (0)

#define FB_LOG_INFO(message) FB_LOG(Info, message)
#define FB_LOG_OK(message) FB_LOG(Ok, message)
#define FB_LOG_WARNING(message) FB_LOG(Warning, message)
#define FB_LOG_ERROR(message) FB_LOG(Error, message)
#define FB_LOG_FATAL(message) FB_LOG(Fatal, message)

#define FB_LOG_INFO_F(message, ...) FB_LOG_F(Info, message, __VA_ARGS__)
#define FB_LOG_OK_F(message, ...) FB_LOG_F(Ok, message, __VA_ARGS__)
#define FB_LOG_WARNING_F(message, ...) FB_LOG_F(Warning, message, __VA_ARGS__)
#define FB_LOG_ERROR_F(message, ...) FB_LOG_F(Error, message, __VA_ARGS__)
#define FB_LOG_FATAL_F(message, ...) FB_LOG_F(Fatal, message, __VA_ARGS__)

#ifdef F_DEBUG
#define FB_LOG_DEBUG(message) FB_LOG(Debug, message)
#define FB_LOG_DEBUG_F(message, ...) FB_LOG_F(Debug, message, __VA_ARGS__)
#else
#define FB_LOG_DEBUG(message)
#define FB_LOG_DEBUG_F(message, ...)
#endif

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP
