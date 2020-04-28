#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
#error "Include Functional.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC

#ifndef F_NO_CUSTOM_INITIALIZER_LIST
#include "IntegerTypes.hpp"

// NOLINTNEXTLINE(cert-dcl58-cpp)
namespace std {

    /**
     * Standard initializer_list implementation.
     */
    template <typename T>
    class initializer_list {
       public:
        using Iterator = const T*;
        using ConstIterator = const T*;

       public:
        /**
         * Creates an empty initializer_list.
         */
        constexpr initializer_list() noexcept : m_data(0) {}

        /**
         * Returns an iterator pointing at the beginning of the list.
         *
         * @return an iterator pointing at the beginning of the list.
         */
        constexpr Iterator Begin() const noexcept {
            return m_data;
        }

        /**
         * Returns an iterator pointing at the end of the list. (The element 1 after the last element of the list).
         *
         * @return an iterator pointing at the end of the list. (The element 1 after the last element of the list).
         */
        constexpr Iterator End() const noexcept {
            return m_data + m_length;
        }

        HAS_STANDARD_ITERATORS;

       private:
        /** used internally by compiler */
        constexpr initializer_list(const T* data, FunnyOS::Stdlib::size_t length) F_UNUSED : m_data(data),
                                                                                             m_length(length) {}

       private:
        const T* m_data;
        FunnyOS::Stdlib::size_t m_length{0};
    };
}  // namespace std
#endif

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INITIALIZERLIST_TCC
