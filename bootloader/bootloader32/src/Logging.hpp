#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

namespace FunnyOS::Bootloader32::Logging {

    Stdlib::Ref<Stdlib::FilteringLoggingSink>& GetVgaOutputSink();

    Stdlib::Ref<Stdlib::FilteringLoggingSink>& GetSerialLoggingSink();

    void InitLogging();

    /**
     * Gets the TerminalManager instance used by the logging system.
     */
    Stdlib::Ref<Misc::TerminalManager::TerminalManager>& GetTerminalManager();

    Stdlib::Logger& GetLogger();

}  // namespace FunnyOS::Bootloader32::Logging

#define FB_LOGGER() FunnyOS::Bootloader32::Logging::GetLogger()

#define FB_LOG_INFO(message) F_LOG_INFO(FB_LOGGER(), message)
#define FB_LOG_OK(message) F_LOG_OK(FB_LOGGER(), message)
#define FB_LOG_WARNING(message) F_LOG_WARNING(FB_LOGGER(), message)
#define FB_LOG_ERROR(message) F_LOG_ERROR(FB_LOGGER(), message)
#define FB_LOG_FATAL(message) F_LOG_FATAL(FB_LOGGER(), message)
#define FB_LOG_DEBUG(message) F_LOG_DEBUG(FB_LOGGER(), message)

#define FB_LOG_INFO_F(message, ...) F_LOG_INFO_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_OK_F(message, ...) F_LOG_OK_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_WARNING_F(message, ...) F_LOG_WARNING_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_ERROR_F(message, ...) F_LOG_ERROR_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_FATAL_F(message, ...) F_LOG_FATAL_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_DEBUG_F(message, ...) F_LOG_DEBUG_F(FB_LOGGER(), message, __VA_ARGS__)

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_LOGGING_HPP
