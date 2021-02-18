#include <FunnyOS/Stdlib/System.hpp>

#include <FunnyOS/Stdlib/Platform.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/String.hpp>

namespace FunnyOS::Stdlib::System {

    Exception::~Exception() noexcept = default;

    const char* Exception::GetMessage() const noexcept {
        return "FunnyOS::Stdlib::System::Exception";
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
        String::Format(errorBuffer, error, &args);
        va_end(args);

        _Platform::ReportError(errorBuffer.Data);
    }

    namespace _Exception {

        // TODO: Thread local
        static Exception* s_activeException = nullptr;

        Exception* GetAndClearActiveException() {
            Exception* exception = s_activeException;
            s_activeException    = nullptr;
            return exception;
        }

        Exception* SetActiveException(Exception* ptr) {
            delete s_activeException;

            return (s_activeException = ptr);
        }
    }  // namespace _Exception
}  // namespace FunnyOS::Stdlib::System