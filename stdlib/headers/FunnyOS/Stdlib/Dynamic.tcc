#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
#error "Include Dynamic.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(const SmartSizedBuffer& other) {
        this->Data = nullptr;
        Memory::ReallocateBuffer(*this, other.Size);
        Memory::Copy(*this, other.Data);
    }

    template <typename T>
    SmartSizedBuffer<T>& SmartSizedBuffer<T>::operator=(const SmartSizedBuffer& other) {
        if (&other == this) {
            return *this;
        }

        Memory::ReallocateBuffer(*this, other.Size);
        Memory::Copy(*this, other.Data);
        return *this;
    }

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(SmartSizedBuffer&& other) noexcept {
        Memory::FreeBuffer(*this);
        this->Data = other.Data;
        this->Size = other.Size;
        other.Data = nullptr;
        other.Size = 0;
    }

    template <typename T>
    SmartSizedBuffer<T>& SmartSizedBuffer<T>::operator=(SmartSizedBuffer&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        Memory::FreeBuffer(*this);
        this->Data = other.Data;
        this->Size = other.Size;
        other.Data = nullptr;
        other.Size = 0;

        return *this;
    }

    template <typename T>
    SmartSizedBuffer<T>::SmartSizedBuffer(size_t size) {
        this->Data = nullptr;
        Memory::ReallocateBuffer(*this, size);
    }

    template <typename T>
    SmartSizedBuffer<T>::~SmartSizedBuffer() {
        Memory::FreeBuffer(*this);
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_TCC
