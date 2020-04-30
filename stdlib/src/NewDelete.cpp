#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/System.hpp>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-dcl54-cpp"
#pragma ide diagnostic ignored "cert-dcl58-cpp"
#pragma ide diagnostic ignored "fuchsia-overloaded-operator"

// Types required for the operators
namespace std {
    namespace {
        using size_t = FunnyOS::Stdlib::size_t;

        enum class align_val_t : std::size_t {};

        struct nothrow_t {
            explicit nothrow_t() = default;
        };

        std::nothrow_t nothrow;
    }  // namespace
}  // namespace std

//
// Implementations of various new and delete operator.
//
using namespace FunnyOS::Stdlib;

[[nodiscard]] void* operator new(std::size_t size, const std::nothrow_t& /*unused*/) noexcept {
    return Memory::Allocate(size);
}

[[nodiscard]] void* operator new(std::size_t size, std::align_val_t alignment,
                                 const std::nothrow_t& /*unused*/) noexcept {
    return Memory::AllocateAligned(size, static_cast<size_t>(alignment));
}

[[nodiscard]] void* operator new(std::size_t size) {
    void* mem = operator new(size, std::nothrow);
    if (mem == nullptr) {
        F_ERROR(System::BadAllocation);
    }
    return mem;
}

[[nodiscard]] void* operator new(std::size_t size, std::align_val_t alignment) {
    void* mem = operator new(size, alignment, std::nothrow);
    if (mem == nullptr) {
        F_ERROR(System::BadAllocation);
    }
    return mem;
}

[[nodiscard]] void* operator new[](std::size_t size) {
    return operator new(size);
}

[[nodiscard]] void* operator new[](std::size_t size, std::align_val_t alignment) {
    return operator new(size, alignment);
}

[[nodiscard]] void* operator new[](std::size_t size, const std::nothrow_t& /*unused*/) noexcept {
    return operator new(size, std::nothrow);
}

[[nodiscard]] void* operator new[](std::size_t size, std::align_val_t alignment,
                                   const std::nothrow_t& /*unused*/) noexcept {
    return operator new(size, alignment, std::nothrow);
}

[[nodiscard]] void* operator new(std::size_t /*size*/, void* ptr) noexcept {
    return ptr;
}

[[nodiscard]] void* operator new[](std::size_t size, void* ptr) noexcept {
    return operator new(size, ptr);
}

void operator delete(void* ptr) noexcept {
    Memory::Free(ptr);
}

void operator delete(void* ptr, std::size_t /*unused*/) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::align_val_t /*unused*/) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::size_t /*unused*/, std::align_val_t /*unused*/) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& /*unused*/) noexcept {
    operator delete(ptr);
}

void operator delete(void* ptr, std::align_val_t /*unused*/, const std::nothrow_t& /*unused*/) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept {
    operator delete(ptr, size);
}

void operator delete[](void* ptr, std::align_val_t alignment) noexcept {
    operator delete(ptr, alignment);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept {
    operator delete(ptr, size, alignment);
}

void operator delete[](void* ptr, const std::nothrow_t& /*unused*/) noexcept {
    operator delete(ptr, std::nothrow);
}

void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t& /*unused*/) noexcept {
    operator delete(ptr, alignment, std::nothrow);
}

void operator delete(void* ptr, void* /*place*/) noexcept {
    operator delete(ptr);
}

void operator delete[](void* ptr, void* place) noexcept {
    operator delete(ptr, place);
}

#pragma clang diagnostic pop