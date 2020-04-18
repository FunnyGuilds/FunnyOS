#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP
#error "Include Memory.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_TCC

namespace FunnyOS::Stdlib::Memory {

    template <typename T>
    inline bool SizedBuffer<T>::IsValid() {
        return Data != nullptr;
    }

    template <typename T>
    inline T* SizedBuffer<T>::operator[](size_t index) {
        return Data + index;
    }

    template <typename T>
    inline T const* SizedBuffer<T>::operator[](size_t index) const {
        return Data + index;
    }

    template <typename Type>
    inline void Copy(SizedBuffer<Type>& destination, const Type* source) {
        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = *(source + i);
        }
    }

    inline void Copy(void* destination, const void* source, size_t size) {
        Copy(static_cast<uint8_t*>(destination), static_cast<const uint8_t*>(source), size);
    }

    template <typename Type>
    inline void Copy(Type* destination, const Type* source, size_t size) {
        for (size_t i = 0; i < size; i++) {
            *(destination + i) = *(source + i);
        }
    }

    template <typename Type>
    inline void Move(SizedBuffer<Type>& destination, const Type* source) {
        if (destination.Data < source) {
            for (size_t i = 0; i < destination.Size; i++) {
                *(destination.Data + i) = *(source + i);
            }
        } else if (destination.Data > source) {
            for (ssize_t i = destination.Size - 1; i >= 0; i--) {
                *(destination.Data + i) = *(source + i);
            }
        }
    }

    inline void Move(void* destination, const void* source, size_t size) {
        SizedBuffer<uint8_t> buffer = {static_cast<uint8_t*>(destination), size};
        Move(buffer, static_cast<const uint8_t*>(source));
    }

    template <typename Type>
    inline void Set(SizedBuffer<Type>& destination, Type byte) {
        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = byte;
        }
    }

    template <typename Type>
    inline void Fill(SizedBuffer<Type>& destination, const SizedBuffer<Type>& pattern) {
        for (size_t i = 0; i < destination.Size; i++) {
            *(destination.Data + i) = *(pattern.Data + (i % pattern.Size));
        }
    }

    template <typename T>
    inline SizedBuffer<T> AllocateBuffer(size_t size) {
        auto* data = static_cast<T*>(Allocate(size * sizeof(T)));
        return SizedBuffer<T>{data, data == nullptr ? 0 : size};
    }

    template <typename T>
    [[nodiscard]] inline SizedBuffer<T> AllocateBufferInitialized(size_t size) {
        auto* data = static_cast<T*>(AllocateInitialized(size * sizeof(T)));
        return SizedBuffer<T>{data, data == nullptr ? 0 : size};
    }

    template <typename T>
    [[nodiscard]] inline SizedBuffer<T> AllocateBufferAligned(size_t size, size_t alignment) {
        auto* data = static_cast<T*>(AllocateAligned(size * sizeof(T), alignment));
        return SizedBuffer<T>{data, data == nullptr ? 0 : size};
    }

    template <typename T>
    [[nodiscard]] inline SizedBuffer<T> AllocateBufferAlignedAndInitialized(size_t size, size_t alignment) {
        auto* data = static_cast<T*>(AllocateAlignedAndInitialized(size * sizeof(T), alignment));
        return SizedBuffer<T>{data, data == nullptr ? 0 : size};
    }

    template <typename T>
    inline void ReallocateBuffer(SizedBuffer<T>& buffer, size_t size) {
        auto* data = static_cast<T*>(Reallocate(buffer.Data, size * sizeof(T)));
        if (data == nullptr) {
            FreeBuffer(buffer);
            return;
        }

        buffer.Data = data;
        buffer.Size = size;
    }

    template <typename T>
    inline void FreeBuffer(SizedBuffer<T>& buffer) {
        Memory::Free(buffer.Data);
        buffer.Data = nullptr;
        buffer.Size = 0;
    }

}  // namespace FunnyOS::Stdlib::Memory

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP
