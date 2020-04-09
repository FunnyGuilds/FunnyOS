#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-dcl58-cpp"
#pragma ide diagnostic ignored "fuchsia-overloaded-operator"

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_NEW_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_NEW_HPP

#include "IntegerTypes.hpp"

// Placement new
#ifndef F_NO_PLACEMENT_NEW
namespace std {
    using size_t = FunnyOS::Stdlib::size_t;
}

[[nodiscard]] void* operator new(std::size_t size, void* ptr) noexcept;
[[nodiscard]] void* operator new[](std::size_t size, void* ptr) noexcept;
#endif

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_NEW_HPP

#pragma clang diagnostic pop