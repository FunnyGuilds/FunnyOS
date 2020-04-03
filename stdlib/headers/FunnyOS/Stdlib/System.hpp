#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_SYSTEM_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_SYSTEM_HPP

#include "Compiler.hpp"

//
// Assertion macros
//

#ifdef F_ASSERTIONS_THROW_EXCEPTIONS
#define F_FAIL_ASSERT(message)                                                          \
    do {                                                                                \
        throw FunnyOS::Stdlib::System ::AssertionFailure("Assertion failed: " message); \
    } while (0)
#else
#define F_FAIL_ASSERT(message)                                             \
    do {                                                                   \
        FunnyOS::Stdlib::System ::Terminate("Assertion failed: " message); \
    } while (0)
#endif

#define F_ASSERT(condition, message) \
    do {                             \
        if (!(condition)) {          \
            F_FAIL_ASSERT(message);  \
        }                            \
    } while (0)

namespace FunnyOS::Stdlib::System {

    /**
     * Aborts the current process execution.
     */
    [[noreturn]] void Abort();

    /**
     * Terminates the current process with the supplied reason.
     */
    [[noreturn]] void Terminate(const char* reason);

    /**
     * Reports error to the standard process error stream.
     * String is formatted via String::Format
     *
     * @param error error format
     * @param ... format parameters
     */
    void ReportError(const char* error, ...);

    /**
     * Base class for all exception types.
     */
    class Exception {
       public:
        Exception() noexcept = default;
        virtual ~Exception() noexcept;

        Exception(const Exception&) = default;
        Exception& operator=(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(Exception&&) = default;

        /**
         * Get the message of the exception.
         */
        [[nodiscard]] virtual const char* GetMessage() const noexcept;
    };

    /**
     * Exception thrown by dynamic_cast when a bad reference cast happen.
     */
    class BadCastException : public Exception {
       public:
        BadCastException() noexcept = default;
        ~BadCastException() override;

        BadCastException(const BadCastException&) = default;
        BadCastException& operator=(const BadCastException&) = default;
        BadCastException(BadCastException&&) = default;
        BadCastException& operator=(BadCastException&&) = default;

        [[nodiscard]] const char* GetMessage() const noexcept override;
    };

    /**
     * An exception of this type is thrown when a typeid operator is applied to a dereferenced null pointer value.
     */
    class BadTypeidException : public Exception {
       public:
        BadTypeidException() noexcept = default;
        ~BadTypeidException() override;

        BadTypeidException(const BadTypeidException&) = default;
        BadTypeidException& operator=(const BadTypeidException&) = default;
        BadTypeidException(BadTypeidException&&) = default;
        BadTypeidException& operator=(BadTypeidException&&) = default;

        [[nodiscard]] const char* GetMessage() const noexcept override;
    };

    /**
     * Exception thrown by the allocation functions to report failure to allocate storage.
     */
    class BadAllocation : public Exception {
       public:
        BadAllocation() noexcept = default;
        ~BadAllocation() override;

        BadAllocation(const BadAllocation&) = default;
        BadAllocation& operator=(const BadAllocation&) = default;
        BadAllocation(BadAllocation&&) = default;
        BadAllocation& operator=(BadAllocation&&) = default;

        [[nodiscard]] const char* GetMessage() const noexcept override;
    };

    /**
     * Thrown by F_ASSERT if an assertion fails and F_ASSERTIONS_THROW_EXCEPTIONS is defined.
     */
    class AssertionFailure : public Exception {
       public:
        AssertionFailure(const char* message) noexcept;
        ~AssertionFailure() override;

        AssertionFailure(const AssertionFailure&) = default;
        AssertionFailure& operator=(const AssertionFailure&) = default;
        AssertionFailure(AssertionFailure&&) = default;
        AssertionFailure& operator=(AssertionFailure&&) = default;

        [[nodiscard]] const char* GetMessage() const noexcept override;

       private:
        const char* m_message;
    };

}  // namespace FunnyOS::Stdlib::System

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_SYSTEM_HPP
