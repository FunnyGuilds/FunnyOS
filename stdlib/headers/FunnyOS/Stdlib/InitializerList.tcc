#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
#error "Include Functional.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC

#ifndef F_NO_CUSTOM_INITIALIZER_LIST
#include "IntegerTypes.hpp"

// NOLINTNEXTLINE(cert-dcl58-cpp)
namespace std {
    template <typename T>
    class initializer_list {
       public:
        using Iterator = const T*;
        using ConstIterator = const T*;

       public:
        constexpr initializer_list() noexcept : m_data(0), m_length(0) {}

        constexpr Iterator Begin() const {
            return m_data;
        }

        constexpr Iterator End() const {
            return m_data + m_length;
        }

        HAS_STANDARD_ITERATORS;

       private:
        constexpr initializer_list(const T* data, FunnyOS::Stdlib::size_t length) : m_data(data), m_length(length) {}

       private:
        const T* m_data;
        FunnyOS::Stdlib::size_t m_length;
    };
}  // namespace std
#endif

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC
