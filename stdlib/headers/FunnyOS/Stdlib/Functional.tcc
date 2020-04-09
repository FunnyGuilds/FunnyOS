#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
#error "Include Functional.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC

#include "New.hpp"

namespace FunnyOS::Stdlib {
    template <typename T>
    Storage<T>::Storage() noexcept : m_initialized(false){};

    template <typename T>
    Storage<T>::Storage(T&& value) : m_initialized(true) {
        new (static_cast<void*>(&m_data)) T(value);
    }

    template <typename T>
    template <typename... Args>
    Storage<T>::Storage(const InPlaceConstructor* /*unused*/, Args&&... args) : m_initialized(true) {
        new (static_cast<void*>(&m_data)) T(Forward<Args>(args)...);
    }

    template <typename T>
    Storage<T>::~Storage() {
        if (m_initialized) {
            Destroy();
        }
    }

    template <typename T>
    void Storage<T>::Destroy() {
        GetObject().~T();
        m_initialized = false;
    }

    template <typename T>
    Storage<T>::Storage(const Storage& other) : m_initialized(other.m_initialized) {
        if (other.m_initialized) {
            new (static_cast<void*>(&m_data)) T(static_cast<const T&>(other.GetObject()));
        }
    }

    template <typename T>
    Storage<T>& Storage<T>::operator=(const Storage& other) {
        if (other == this) {
            return *this;
        }

        if (m_initialized) {
            Destroy();
        }

        if (other.m_initialized) {
            GetObject() = static_cast<const T&>(other.GetObject());
        }

        return *this;
    }

    template <typename T>
    Storage<T>::Storage(Storage&& other) noexcept : m_initialized(other.m_initialized) {
        if (other.m_initialized) {
            new (static_cast<void*>(&m_data)) T(static_cast<T&&>(other.GetObject()));
            other.Destroy();
        }
    }

    template <typename T>
    Storage<T>& Storage<T>::operator=(Storage&& other) noexcept {
        if (other == this) {
            return *this;
        }

        if (m_initialized) {
            Destroy();
        }

        if (other.m_initialized) {
            GetObject() = static_cast<T&&>(other.GetObject());

            other.Destroy();
        }

        return *this;
    }

    template <typename T>
    T& Storage<T>::GetObject() {
        T* ptr = reinterpret_cast<T*>(&m_data);
        return reinterpret_cast<T&>(*ptr);
    }

    template <typename T>
    const T& Storage<T>::GetObject() const {
        const T* ptr = reinterpret_cast<const T*>(&m_data);
        return reinterpret_cast<const T&>(*ptr);
    }

    template <typename T>
    bool Storage<T>::IsInitialized() const {
        return m_initialized;
    }

    template <typename T>
    Optional<T>::Optional() noexcept : m_storage() {}

    template <typename T>
    Optional<T>::Optional(const NullOptionalType* /*unused*/) noexcept : m_storage() {}

    template <typename T>
    template <typename... Args>
    Optional<T>::Optional(const InPlaceConstructor* inPlace, Args&&... args)
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
            throw EmptyOptionalException();
        }

        return m_storage.GetObject();
    }

    template <typename T>
    const T& Optional<T>::GetValue() const {
        if (!HasValue()) {
            throw EmptyOptionalException();
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
    Optional<T>::operator bool() {
        return HasValue();
    }

    template <typename T>
    Optional<T>::operator bool() const {
        return HasValue();
    }

    template <typename T>
    inline Optional<T> EmptyOptional() noexcept {
        return Optional<T>{NullOptionalType::Value};
    }

    template <typename T, typename... Args>
    inline Optional<T> MakeOptional(Args&&... args) {
        return Optional<T>(InPlaceConstructor::Value, Forward(args)...);
    }
}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_TCC
