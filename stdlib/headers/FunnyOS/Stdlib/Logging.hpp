#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_HPP

#include "Compiler.hpp"
#include "Dynamic.hpp"
#include "Vector.hpp"

#define F_LOG(logger, level, message)                                     \
    do {                                                                  \
        logger.PostRawMessage(FunnyOS::Stdlib::LogLevel::level, message); \
    } while (0)

#define F_LOG_F(logger, level, message, ...)                                        \
    do {                                                                            \
        logger.PostMessage(FunnyOS::Stdlib::LogLevel::level, message, __VA_ARGS__); \
    } while (0)

#define F_LOG_INFO(logger, message)    F_LOG(logger, Info, message)
#define F_LOG_OK(logger, message)      F_LOG(logger, Ok, message)
#define F_LOG_WARNING(logger, message) F_LOG(logger, Warning, message)
#define F_LOG_ERROR(logger, message)   F_LOG(logger, Error, message)
#define F_LOG_FATAL(logger, message)   F_LOG(logger, Fatal, message)
#define F_LOG_DEBUG(logger, message)   F_LOG(logger, Debug, message)

#define F_LOG_INFO_F(logger, message, ...)    F_LOG_F(logger, Info, message, __VA_ARGS__)
#define F_LOG_OK_F(logger, message, ...)      F_LOG_F(logger, Ok, message, __VA_ARGS__)
#define F_LOG_WARNING_F(logger, message, ...) F_LOG_F(logger, Warning, message, __VA_ARGS__)
#define F_LOG_ERROR_F(logger, message, ...)   F_LOG_F(logger, Error, message, __VA_ARGS__)
#define F_LOG_FATAL_F(logger, message, ...)   F_LOG_F(logger, Fatal, message, __VA_ARGS__)
#define F_LOG_DEBUG_F(logger, message, ...)   F_LOG_F(logger, Debug, message, __VA_ARGS__)

namespace FunnyOS::Stdlib {

    /**
     * Severity level of a log message.
     */
    enum class LogLevel { Debug, Info, Ok, Warning, Error, Fatal, Off, Default = Info };

    /**
     * Returns a string representation of a LogLevel element.
     *
     * @param level log level
     * @return the string representation of that LogLevel
     */
    const char* GetLogLevelName(LogLevel level);

    /**
     * Represents a logging sink.
     * Sinks are used to output a log message (i.e. to a terminal or to a file).
     */
    class ILoggingSink {
        INTERFACE(ILoggingSink);

       public:
        /**
         * Submits a message to the sink.
         *
         * @param level severity level of the message
         * @param message raw message to output
         */
        virtual void SubmitMessage(LogLevel level, const char* message) = 0;
    };

    /**
     * Represents a sink that outputs to another sink only when message's severity level is greater or equal to
     * the level specified by [GetLevel()].
     */
    class FilteringLoggingSink : public ILoggingSink {
       public:
        /**
         * Constructs a new FilteringLoggingSink that will output to [sink] when a log level of a message is greater or
         * equal to the LogLevel::Default level.
         *
         * @param sink sink to output to
         */
        FilteringLoggingSink(Ref<ILoggingSink> sink);

        /**
         * Constructs a new FilteringLoggingSink that will output to [sink] when a log level of a message is greater or
         * equal to [level].
         *
         * @param sink sink to output to
         */
        FilteringLoggingSink(Ref<ILoggingSink> sink, LogLevel level);

        /**
         * Submits a message to the underlying sink only if the [level] is greater or equal to the level specified by
         * [GetLevel()]
         *
         * @param level severity level of the message
         * @param message[in] raw message to output
         */
        void SubmitMessage(LogLevel level, const char* message) override;

        /**
         * @return the minimum level required to for a message to be submitted to this logger.
         */
        [[nodiscard]] LogLevel GetLevel() const;

        /**
         * Sets the minimum level required to for a message to be submitted to this logger.
         * @param level minimum level
         */
        void SetLevel(LogLevel level);

       private:
        Ref<ILoggingSink> m_sink;
        LogLevel m_level;
    };

    /**
     * Posts a new message to the logging system formatted via String::Format
     *
     * @param[in] level severity of the message
     * @param[in] format message format
     * @param[in] args String::Format parameters.
     */
    class Logger {
       public:
        /**
         * Posts a new message to this logger.
         *
         * @param level severity of the message
         * @param[in] message content of the message
         */
        void PostRawMessage(LogLevel level, const char* message);

        /**
         * Posts a new message to this logger formatted via String::Format.
         *
         * @param level severity of the message
         * @param[in] message content of the message
         * @param ... String::Format parameters.
         */
        void PostMessage(LogLevel level, const char* format, ...);

        /**
         * Posts a new message to this logger formatted via String::Format.
         *
         * @param level severity of the message
         * @param[in] message content of the message
         * @param args String::Format parameters.
         */
        void PostMessage(LogLevel level, const char* format, va_list* args);

        /**
         * Adds a new sink to this logger.
         *
         * @param sink sink to be added.
         */
        void AddSink(const Ref<ILoggingSink>& sink);

        /**
         * Removes a sink from this logger.
         *
         * @param[in] sink sink to be removed.
         */
        void RemoveSink(const ILoggingSink* sink);

       private:
        Vector<Ref<ILoggingSink>> m_sinks{};
    };

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_HPP
