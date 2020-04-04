#include <FunnyOS/Stdlib/System.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>

#include <FunnyOS/Stdlib/Platform.hpp>

namespace FunnyOS::Stdlib::System {

    Exception::~Exception() noexcept = default;

    const char* Exception::GetMessage() const noexcept {
        return "FunnyOS::Stdlib::System::Exception";
    }
    BadCastException::~BadCastException() = default;

    const char* BadCastException::GetMessage() const noexcept {
        return "FunnyOS::Stdlib::System::BadCastException";
    }

    BadTypeidException::~BadTypeidException() = default;

    const char* BadTypeidException::GetMessage() const noexcept {
        return "FunnyOS::Stdlib::System::BadTypeidException";
    }

    BadAllocation::~BadAllocation() = default;

    const char* BadAllocation::GetMessage() const noexcept {
        return "FunnyOS::Stdlib::System::BadAllocation";
    }

    AssertionFailure::AssertionFailure(const char* message) noexcept : m_message(message) {}

    AssertionFailure::~AssertionFailure() = default;

    const char* AssertionFailure::GetMessage() const noexcept {
        return m_message;
    }

    void Abort() {
        Terminate("Abort() called");
    }

    void Terminate(const char* reason) {
        _Platform::Terminate(reason);
    }

    void ReportError(const char* error, ...) {
        static char errorBufferMemory[256];
        Memory::SizedBuffer<char> errorBuffer{errorBufferMemory, 256};

        va_list args;
        va_start(args, error);
        String::Format(errorBuffer, error, args);
        va_end(args);

        _Platform::ReportError(errorBuffer.Data);
    }
}  // namespace FunnyOS::Stdlib::System