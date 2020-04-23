#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_TCC
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_HPP
#error "Include Vector.hpp instaed"
#endif

#include "Memory.hpp"
#include "New.hpp"
#include "Functional.hpp"
#include "Vector.hpp"
#include "String.hpp"

namespace FunnyOS::Stdlib {

    template <typename T>
    Vector<T>::Vector(const Vector& other) : m_growthFactor(other.m_growthFactor), m_size(0), m_data({nullptr, 0}) {
        EnsureCapacityExact(other.Size());

        for (auto const& element : other) {
            Append(element);
        }
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(const Vector& other) {
        Clear();
        EnsureCapacityExact(other.Size());

        for (auto const& element : other) {
            Append(element);
        }
    }

    template <typename T>
    Vector<T>::Vector(Vector&& other) noexcept
        : m_growthFactor(other.m_growthFactor), m_size(other.m_size), m_data(other.m_data) {
        other.m_data.Data = nullptr;
    }

    template <typename T>
    Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
        Clear();
        Memory::FreeBuffer(m_data);

        m_growthFactor = other.m_growthFactor;
        m_size = other.m_size;
        m_data = other.m_data;

        other.m_data.Data = nullptr;
    }

    template <typename T>
    Vector<T>::Vector() : Vector(0) {}

    template <typename T>
    Vector<T>::Vector(size_t initialCapacity) : Vector(initialCapacity, DEFAULT_GROWTH_FACTOR) {}

    template <typename T>
    Vector<T>::Vector(size_t initialCapacity, float growthFactor)
        : m_growthFactor(growthFactor), m_size(0), m_data({nullptr, 0}) {
        F_ASSERT(m_growthFactor > 1, "vector's growth factor too small");

        EnsureCapacityExact(initialCapacity);
    }

    template <typename T>
    Vector<T>::Vector(InitializerList<T> list) : Vector(SizeOf(list), DEFAULT_GROWTH_FACTOR) {
        for (auto& ref : list) {
            Append(ref);
        }
    }

    template <typename T>
    Vector<T>::~Vector() {
        if (m_data.Data != nullptr) {
            Clear();
            Memory::FreeBuffer(m_data);
        }
    }

    template <typename T>
    size_t Vector<T>::Size() const noexcept {
        return m_size;
    }

    template <typename T>
    size_t Vector<T>::Capacity() const noexcept {
        return m_data.Size;
    }

    template <typename T>
    T& Vector<T>::operator[](size_t index) {
        CheckBounds(index);
        return m_data.Data[index];
    }

    template <typename T>
    const T& Vector<T>::operator[](size_t index) const {
        CheckBounds(index);
        return m_data.Data[index];
    }

    template <typename T>
    T& Vector<T>::Head() {
        CheckBounds(0);
        return m_data.Data[0];
    }

    template <typename T>
    T& Vector<T>::Tail() {
        CheckBounds(0);
        return m_data.Data[m_size - 1];
    }

    template <typename T>
    const T& Vector<T>::Head() const {
        CheckBounds(0);
        return m_data.Data[0];
    }

    template <typename T>
    const T& Vector<T>::Tail() const {
        CheckBounds(0);
        return m_data.Data[m_size - 1];
    }

    template <typename T>
    void Vector<T>::Append(const T& value) {
        EnsureCapacity(m_size + 1);
        new (m_data[m_size]) T(value);
        m_size++;
    }

    template <typename T>
    void Vector<T>::Append(T&& value) {
        EnsureCapacity(m_size + 1);
        new (m_data[m_size]) T(value);
        m_size++;
    }

    template <typename T>
    template <typename... Args>
    void Vector<T>::AppendInPlace(Args&&... args) {
        EnsureCapacity(m_size + 1);
        new (m_data[m_size]) T(Forward<Args>(args)...);
        m_size++;
    }

    template <typename T>
    void Vector<T>::Remove(size_t index) {
        CheckBounds(index);

        m_data[index]->~T();

        Memory::SizedBuffer<T> destination{m_data.Data + index, m_size - index - 1};
        Memory::Move(destination, m_data.Data + index + 1);
        m_size--;
    }

    template <typename T>
    void Vector<T>::RemoveRange(size_t from, size_t to) {
        CheckBounds(from);
        CheckBounds(to);

        if (from > to) {
            return;
        }

        for (size_t i = from; i <= to; i++) {
            m_data[i]->~T();
        }

        Memory::SizedBuffer<T> destination{m_data.Data + from, m_size - to - 1};
        Memory::Move(destination, m_data.Data + to + 1);

        m_size -= to - from + 1;
    }

    template <typename T>
    void Vector<T>::ShrinkToSize() {
        Memory::ReallocateBuffer(m_data, m_size);

        if (m_data.Data == nullptr) {
            throw VectorNotEnoughMemory("vector could not allocate enough memory while shrinking");
        }
    }

    template <typename T>
    void Vector<T>::EnsureCapacity(size_t num) {
        if (m_data.Size >= num) {
            return;
        }

        size_t desiredCapacity = m_data.Size;
        while (desiredCapacity < num) {
            desiredCapacity = static_cast<size_t>((static_cast<float>(desiredCapacity) * m_growthFactor)) + 1;
        }

        EnsureCapacityExact(desiredCapacity);
    }

    template <typename T>
    void Vector<T>::Insert(size_t index, const T& value) {
        EnsureCapacity(m_size + 1);
        Shift(index, 1);
        new (m_data.Data + index) T(value);
        m_size++;
    }

    template <typename T>
    void Vector<T>::Insert(size_t index, T&& value) {
        EnsureCapacity(m_size + 1);
        Shift(index, 1);
        new (m_data.Data + index) T(value);
        m_size++;
    }

    template <typename T>
    template <typename... Args>
    void Vector<T>::InsertInPlace(size_t index, Args&&... args) {
        EnsureCapacity(m_size + 1);
        Shift(index, 1);
        new (m_data.Data + index) T(Forward<Args>(args)...);
        m_size++;
    }

    template <typename T>
    template <class OtherIterator>
    void Vector<T>::Insert(size_t index, OtherIterator value, size_t size) {
        EnsureCapacity(m_size + size);
        Shift(index, size);

        for (size_t i = index; i < index + size; i++) {
            new (m_data.Data + i) T(*value);
            value++;
        }

        m_size += size;
    }

    template <typename T>
    void Vector<T>::Clear() {
        for (size_t i = 0; i < m_size; i++) {
            (m_data.Data + i)->~T();
        }

        m_size = 0;
    }

    template <typename T>
    typename Vector<T>::Iterator Vector<T>::Begin() noexcept {
        return m_data.Data;
    }

    template <typename T>
    typename Vector<T>::Iterator Vector<T>::End() noexcept {
        return m_data.Data + m_size;
    }

    template <typename T>
    typename Vector<T>::ConstIterator Vector<T>::Begin() const noexcept {
        return m_data.Data;
    }

    template <typename T>
    typename Vector<T>::ConstIterator Vector<T>::End() const noexcept {
        return m_data.Data + m_size;
    }

    template <typename T>
    void Vector<T>::CheckBounds(size_t index) const {
        if (index >= m_size) {
            String::StringBuffer errorBuffer = Memory::AllocateBuffer<char>(32);
            String::Format(errorBuffer, "%llu >= %llu", index, m_size);
            throw VectorIndexOutOfBounds(errorBuffer.Data);
        }
    }

    template <typename T>
    void Vector<T>::Shift(size_t index, size_t count) {
        if (count == 0 || index == m_size) {
            return;
        }
        CheckBounds(index);

        Memory::SizedBuffer<T> destination{m_data.Data + index + count, m_size - index};
        Memory::Move(destination, m_data.Data + index);
    }

    template <typename T>
    void Vector<T>::EnsureCapacityExact(size_t desiredCapacity) {
        Memory::ReallocateBuffer(m_data, desiredCapacity);

        if (m_data.Data == nullptr) {
            throw VectorNotEnoughMemory("vector could not allocate enough memory");
        }
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_TCC
