#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP
#error "Include Utility.hpp instaed"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_TCC

namespace FunnyOS::Stdlib {
    template <typename T>
    Storage<T>::Storage() noexcept : m_initialized(false){};

    template <typename T>
    Storage<T>::Storage(const T& value) : m_initialized(true) {
        new (static_cast<void*>(&m_data)) T(value);
    }

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
}

#endif