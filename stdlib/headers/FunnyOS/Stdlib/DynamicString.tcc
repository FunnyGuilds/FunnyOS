
#include "DynamicString.hpp"

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_HPP
#error "Include DynamicString.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    BasicDynamicString<T>::BasicDynamicString() : m_data{} {}

    template <typename T>
    BasicDynamicString<T>::BasicDynamicString(size_t count, const char* pattern) : BasicDynamicString{} {
        m_data.EnsureCapacity(count);

        const size_t patternSize = String::Length(pattern);

        for (size_t i = 0; i < count; i++) {
            m_data.Append(patternSize == 0 ? 0 : pattern[i % patternSize]);
        }

        m_data.Append(0);
    }

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
        return m_data.Size() == 0 ? 0 : (m_data.Size() - 1);
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
        RequireBuffer();
        m_data.Insert(m_data.Size() - 1, value.Begin(), value.Size());
    }

    template <typename T>
    void BasicDynamicString<T>::Append(T character) {
        RequireBuffer();
        m_data.Insert(m_data.Size() - 1, character);
    }

    template <typename T>
    void BasicDynamicString<T>::Replace(const BasicDynamicString<T>& from, const BasicDynamicString<T>& to) {
        if (from.Size() == 0) {
            return;
        }

        size_t matchCount = 0;

        for (size_t i = 0; i < Size(); i++) {
            if (m_data[i] == from[matchCount]) {
                matchCount++;
            } else {
                matchCount = 0;
            }

            if (matchCount == from.Size()) {
                const size_t startingIndex = i - matchCount + 1;

                // we have a match
                m_data.RemoveRange(startingIndex, i);
                m_data.Insert(startingIndex, to.Begin(), to.Size());

                i = startingIndex + to.Size();
            }
        }
    }

    template <typename T>
    void BasicDynamicString<T>::Replace(T from, T to) {
        for (T& c : *this) {
            if (c == from) {
                c = to;
            }
        }
    }

    template <typename T>
    void BasicDynamicString<T>::Trim() {
        Trim("\n\t ");
    }

    template <typename T>
    void BasicDynamicString<T>::Trim(const char* characterSet) {
        if (Size() == 0) {
            return;
        }

        // count leading
        size_t leading = 0;
        while (leading < Size() && String::Matches(m_data[leading], characterSet)) {
            leading++;
        }

        // remove leading
        if (leading != 0) {
            m_data.RemoveRange(0, leading - 1);
        }

        // count trailing
        ssize_t trailingStart = Size() - 1;
        while (trailingStart >= 0 && String::Matches(m_data[trailingStart], characterSet)) {
            trailingStart--;
        }

        // remove trailing
        if (static_cast<size_t>(trailingStart) != Size() - 1) {
            m_data.RemoveRange(trailingStart + 1, Size() - 1);
        }
    }

    template <typename T>
    void BasicDynamicString<T>::ToLowercase() {
        for (T& c : *this) {
            c = String::ToLowercase(c);
        }
    }

    template <typename T>
    void BasicDynamicString<T>::ToUppercase() {
        for (T& c : *this) {
            c = String::ToUppercase(c);
        }
    }

    template <typename T>
    void BasicDynamicString<T>::Clear() {
        m_data.Clear();
    }

    template <typename T>
    Vector<T>& BasicDynamicString<T>::AsVectorView() {
        return m_data;
    }

    template <typename T>
    const Vector<T>& BasicDynamicString<T>::AsVectorView() const {
        return m_data;
    }

    template <typename T>
    const T* BasicDynamicString<T>::AsCString() const {
        if (Size() == 0) {
            return "";
        }

        return m_data.Begin();
    }

    template <typename T>
    bool BasicDynamicString<T>::operator==(const BasicDynamicString<T>& other) const {
        if (m_data.Size() != other.m_data.Size()) {
            return false;
        }

        for (size_t i = 0; i < m_data.Size(); i++) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename T>
    bool BasicDynamicString<T>::operator!=(const BasicDynamicString<T>& other) const {
        return !(*this == other);
    }

    template <typename T>
    BasicDynamicString<T>::operator bool() const {
        return this->m_data.Size() != 0;
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

    template <typename T>
    void BasicDynamicString<T>::RequireBuffer() {
        if (m_data.Size() == 0) {
            m_data.Append(0);
        }
    }

    template <typename T>
    hash_t Hash<BasicDynamicString<T>>::operator()(const BasicDynamicString<T>& obj) {
        hash_t hash = 0;

        for (const T character : obj) {
            hash = 31 * hash + character;
        }

        return hash;
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_TCC
