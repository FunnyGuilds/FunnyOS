#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_HPP
#error "Include Logging.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_TCC

namespace FunnyOS::Stdlib {

    template <class T, typename >
    void Logger::AddSink(const Ref<T>& sink) {
        AddSink(StaticRefCast<ILoggingSink>(sink));
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LOGGING_TCC