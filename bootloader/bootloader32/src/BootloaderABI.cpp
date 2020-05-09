#include <FunnyOS/Stdlib/System.hpp>

using namespace FunnyOS::Stdlib;

//
// Safety checks
//

extern "C" {
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