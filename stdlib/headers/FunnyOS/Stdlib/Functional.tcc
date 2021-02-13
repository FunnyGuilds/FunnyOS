#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
#error "Include Functional.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC

#include "New.hpp"

namespace FunnyOS::Stdlib {

    template <typename T>
    Optional<T>::Optional() noexcept : m_storage() {}

    template <typename T>
    Optional<T>::Optional(const NullOptionalTag* /*unused*/) noexcept : m_storage() {}

    template <typename T>
    template <typename... Args>
    Optional<T>::Optional(const InPlaceConstructorTag* inPlace, Args&&... args)
        : m_storage(inPlace, Forward<Args>(args)...) {}

    template <typename T>
    template <typename U, typename>
    Optional<T>::Optional(U&& value) : m_storage(Forward<T>(value)) {}

    template <typename T>
    bool Optional<T>::HasValue() const noexcept {
        return m_storage.IsInitialized();
    }

    template <typename T>
    T& Optional<T>::GetValue() {
        if (!HasValue()) {
            F_ERROR(EmptyOptionalException);
        }

        return m_storage.GetObject();
    }

    template <typename T>
    const T& Optional<T>::GetValue() const {
        if (!HasValue()) {
            F_ERROR(EmptyOptionalException);
        }

        return m_storage.GetObject();
    }

    template <typename T>
    T Optional<T>::GetValueOrDefault(T&& defaultValue) noexcept {
        if (!HasValue()) {
            return defaultValue;
        }

        return m_storage.GetObject();
    }

    template <typename T>
    T Optional<T>::GetValueOrDefault(T&& defaultValue) const noexcept {
        if (!HasValue()) {
            return defaultValue;
        }

        return m_storage.GetObject();
    }

    template <typename T>
    Optional<T>::operator T&() {
        return GetValue();
    }

    template <typename T>
    Optional<T>::operator const T&() const {
        return GetValue();
    }

    template <typename T>
    Optional<T>::operator bool() noexcept {
        return HasValue();
    }

    template <typename T>
    Optional<T>::operator bool() const noexcept {
        return HasValue();
    }

    template <typename T>
    inline T* Optional<T>::operator->() {
        return &GetValue();
    }

    template <typename T>
    inline const T* Optional<T>::operator->() const {
        return &GetValue();
    }

    template <typename T>
    template <typename U, typename Mapper>
    Optional<U> Optional<T>::Map(Mapper mapper) {
        if (!HasValue()) {
            return EmptyOptional<U>();
        }

        return MakeOptional<U>(mapper(GetValue()));
    }

    template <typename T>
    inline Optional<T> EmptyOptional() noexcept {
        return Optional<T>{NullOptionalTag::Value};
    }

    template <typename T, typename... Args>
    inline Optional<T> MakeOptional(Args&&... args) {
        return Optional<T>(InPlaceConstructorTag::Value, Forward<Args>(args)...);
    }

    template <typename Container>
    typename Container::iterator Begin(Container& container) {
        return container.begin();
    }

    template <typename Container>
    typename Container::iterator End(Container& container) {
        return container.end();
    }

    template <typename Container>
    typename Container::const_iterator Begin(const Container& container) {
        return container.begin();
    }

    template <typename Container>
    typename Container::const_iterator End(const Container& container) {
        return container.end();
    }

    template <typename Container>
    typename Container::const_iterator ConstBegin(const Container& container) {
        return container.begin();
    }

    template <typename Container>
    typename Container::const_iterator ConstEnd(const Container& container) {
        return container.end();
    }

    template <typename Container>
    size_t SizeOf(const Container& container) {
        return ConstEnd(container) - ConstBegin(container);
    }
}  // namespace FunnyOS::Stdlib

template <typename Container>
typename Container::Iterator begin(Container& container) {
    return FunnyOS::Stdlib::Begin(container);
}

template <typename Container>
typename Container::Iterator end(Container& container) {
    return FunnyOS::Stdlib::End(container);
}

template <typename Container>
typename Container::ConstIterator begin(const Container& container) {
    return FunnyOS::Stdlib::ConstBegin(container);
}

template <typename Container>
typename Container::ConstIterator end(const Container& container) {
    return FunnyOS::Stdlib::ConstEnd(container);
}

template <typename Container>
typename Container::ConstIterator cbegin(const Container& container) {
    return FunnyOS::Stdlib::ConstBegin(container);
}

template <typename Container>
typename Container::ConstIterator cend(const Container& container) {
    return FunnyOS::Stdlib::ConstEnd(container);
}

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC
