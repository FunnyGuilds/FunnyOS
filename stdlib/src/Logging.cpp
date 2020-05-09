#include <FunnyOS/Stdlib/Logging.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>

namespace FunnyOS::Stdlib {
    const char* GetLogLevelName(LogLevel level) {
        const char* c_logLevelNames[] = {" DBG", "INFO", " OK ", "WARN", " ERR", "FAIL", "???"};

        return c_logLevelNames[static_cast<unsigned int>(level)];
    }

    void Logger::PostRawMessage(LogLevel level, const char* message) {
        for (auto& sink : m_sinks) {
            sink->SubmitMessage(level, message);
        }
    }

    void Logger::PostMessage(LogLevel level, const char* format, ...) {
        va_list args;
        va_start(args, format);
        PostMessage(level, format, &args);
        va_end(args);
    }

    void Logger::PostMessage(LogLevel level, const char* format, va_list* args) {
        static char c_bufferData[512];
        static Stdlib::String::StringBuffer c_buffer{c_bufferData, 512};

        const bool ret = Stdlib::String::Format(c_buffer, format, args);
        PostRawMessage(level, ret ? c_bufferData : format);
    }

    void Logger::AddSink(const Ref<ILoggingSink>& sink) {
        m_sinks.Append(sink);
    }

    void Logger::RemoveSink(const ILoggingSink* sink) {
        RemoveIf(m_sinks, [sink](const auto& test) { return sink == test.Get(); });
    }

    FilteringLoggingSink::FilteringLoggingSink(Ref<ILoggingSink> sink)
        : FilteringLoggingSink(Move(sink), LogLevel::Default) {}

    FilteringLoggingSink::FilteringLoggingSink(Ref<ILoggingSink> sink, LogLevel level)
        : m_sink(Move(sink)), m_level(level) {}

    void FilteringLoggingSink::SubmitMessage(LogLevel level, const char* message) {
        if (level == LogLevel::Off || level < m_level) {
            return;
        }

        m_sink->SubmitMessage(level, message);
    }

    LogLevel FilteringLoggingSink::GetLevel() const {
        return m_level;
    }

    void FilteringLoggingSink::SetLevel(LogLevel level) {
        m_level = level;
    }

}  // namespace FunnyOS::Stdlib