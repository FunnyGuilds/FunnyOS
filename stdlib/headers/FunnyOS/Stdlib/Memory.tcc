#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP
#error "Include Memory.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_TCC

namespace FunnyOS::Stdlib::Memory {
    template <typename Type> inline void Copy(SizedBuffer<Type>& destination, const Type* source) {
        static_assert(sizeof(Type) == 1, "Type must be 1 byte long");

        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = *(source + i);
        }
    }

    inline void Copy(void* destination, const void* source, size_t size) {
        Copy(static_cast<uint8_t*>(destination), static_cast<const uint8_t*>(source), size);
    }

    template <typename Type> inline void Copy(Type* destination, const Type* source, size_t size) {
        static_assert(sizeof(Type) == 1, "Type must be 1 byte long");

        for (size_t i = 0; i < size; i++) {
            *(destination + i) = *(source + i);
        }
    }

    template <typename Type> inline void Set(SizedBuffer<Type>& destination, Type byte) {
        static_assert(sizeof(Type) == 1, "Type must be 1 byte long");

        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = byte;
        }
    }

    template <typename Type> inline void Fill(SizedBuffer<Type>& destination, const SizedBuffer<Type>& pattern) {
        static_assert(sizeof(Type) == 1, "Type must be 1 byte long");

        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = *(pattern.Data + (i % pattern.Size));
        }
    }

    template <typename T> inline SizedBuffer<T> AllocateBuffer(size_t size) {
        auto* data = static_cast<T*>(Allocate(size));
        return SizedBuffer<T>{.Data = data, .Size = data == nullptr ? 0 : size};
    }

}  // namespace FunnyOS::Stdlib::Memory

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP
