#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_HPP
#error "Include DynamicString.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    BasicDynamicString<T>::BasicDynamicString(const T* data) : BasicDynamicString{data, 0, String::Length(data)} {}

    template <typename T>
    BasicDynamicString<T>::BasicDynamicString(const T* data, size_t offset, size_t length) {
        m_data.EnsureCapacity(length + 1);
        m_data.Insert(0, data + offset, length);
        m_data.Append(0);
    }

    template <typename T>
    BasicDynamicString<T>::BasicDynamicString(const Memory::SizedBuffer<T>& buffer)
        : BasicDynamicString{buffer.Data, 0, buffer.Size} {}

    template <typename T>
    size_t BasicDynamicString<T>::Size() const noexcept {
        return m_data.Size() - 1;
    }

    template <typename T>
    void BasicDynamicString<T>::ShrinkToSize() {
        m_data.ShrinkToSize();
    }

    template <typename T>
    void BasicDynamicString<T>::EnsureCapacity(size_t num) {
        m_data.EnsureCapacity(num);
    }

    template <typename T>
    T& BasicDynamicString<T>::operator[](size_t index) {
        return m_data[index];
    }

    template <typename T>
    const T& BasicDynamicString<T>::operator[](size_t index) const {
        return m_data[index];
    }

    template <typename T>
    void BasicDynamicString<T>::Append(const BasicDynamicString<T>& value) {
        m_data.Insert(m_data.Size() - 1, value.Begin(), value.Size());
    }

    template <typename T>
    typename BasicDynamicString<T>::Iterator BasicDynamicString<T>::Begin() noexcept {
        return m_data.Begin();
    }

    template <typename T>
    typename BasicDynamicString<T>::Iterator BasicDynamicString<T>::End() noexcept {
        return m_data.End() - 1;
    }

    template <typename T>
    typename BasicDynamicString<T>::ConstIterator BasicDynamicString<T>::Begin() const noexcept {
        return m_data.Begin();
    }

    template <typename T>
    typename BasicDynamicString<T>::ConstIterator BasicDynamicString<T>::End() const noexcept {
        return m_data.End() - 1;
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC
