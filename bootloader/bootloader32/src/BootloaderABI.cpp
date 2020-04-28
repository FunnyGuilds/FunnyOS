#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/System.hpp>
#include <FunnyOS/Stdlib/String.hpp>

using namespace FunnyOS::Stdlib;

/**
 * Dummy classes for RTTI for the linker to shut up.
 */
namespace __cxxabiv1 {
    class __class_type_info {
        virtual void Dummy();
    };
    class __si_class_type_info {
        virtual void Dummy();
    };

    // Dummy functions so the compiler creates vtables for these classes.
    void __class_type_info::Dummy() {}
    void __si_class_type_info::Dummy() {}
}  // namespace __cxxabiv1

extern "C" {

//
// Exception-related stuff
//

void* __cxa_allocate_exception(size_t thrown_size) noexcept {
    return Memory::Allocate(thrown_size);
}

void __cxa_free_exception(void* thrown_exception) noexcept {
    return Memory::Free(thrown_exception);
}

void* __cxa_allocate_dependent_exception() noexcept {
    return Memory::Allocate(0);
}

void __cxa_free_dependent_exception(void* dependent_exception) noexcept {
    return Memory::Free(dependent_exception);
}

void* __cxa_get_exception_ptr(void* exceptionObject) noexcept {
    return exceptionObject;
}

void __cxa_throw(void* thrown_exception, void* /*tinfo*/, void (*/*dest*/)(void*)) {
    // We don't have any exception handling and stack unwinding so when an exception is thrown we simply crash.

    static const char* c_prologue = "Exception thrown: ";
    static const char* c_prologueNoMemory = "Exception thrown, allocation failed: ";
    static const char* c_prologueConcatFailed = "Exception thrown, allocation failed, concat failed, uh, we fucked";

    static char c_exceptionSafetyBuffer[255];

    // We can assume that any exception thrown will actually inherit stdlib's System::Exception
    auto* exception = reinterpret_cast<System::Exception*>(thrown_exception);

    Memory::SizedBuffer<char> outputBuffer =
        Memory::AllocateBuffer<char>(String::Length(c_prologue) + String::Length(exception->GetMessage()) + 1);

    bool noMemory = false;
    if (outputBuffer.Data == nullptr) {
        outputBuffer.Data = c_exceptionSafetyBuffer;
        outputBuffer.Size = sizeof(c_exceptionSafetyBuffer) / sizeof(char);
        noMemory = true;
    }

    if (!String::Concat(outputBuffer, noMemory ? c_prologueNoMemory : c_prologue, exception->GetMessage())) {
        System::Terminate(c_prologueConcatFailed);
    }

    System::Terminate(outputBuffer.Data);
}

uint32_t __gxx_personality_v0(int /*unused*/, int /*unused*/, uint64_t /*unused*/, void* /*unused*/, void* /*unused*/) {
    return 4;
}

void _Unwind_Resume(struct _Unwind_Exception* /*unused*/) {}

//
// Safety checks
//

void __stack_chk_fail() {
    System::Terminate("stack check failed");
}

void __cxa_pure_virtual() {
    System::Terminate("pure virtual function call");
}

// Thread-safe guards for initializing static variables, since we don't really have threads in bootloader we don't
// really care about thread-safety.
int __cxa_guard_acquire(const uint32_t* g) {
    return static_cast<int>(*g == 0);
}

void __cxa_guard_release(uint32_t* g) {
    *g = 1;
}

void __cxa_guard_abort(uint32_t* /*unused*/) {}
}

// Global destruction
void* __dso_handle = nullptr;

// Shared library stuff
extern "C" int __cxa_atexit(void (*/*unused*/)(void*), void* /*unused*/, void* /*unused*/) {
    return 0;
}