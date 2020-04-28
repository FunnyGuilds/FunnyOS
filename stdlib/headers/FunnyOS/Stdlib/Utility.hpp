#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP

#include "TypeTraits.hpp"

namespace FunnyOS::Stdlib {

    /**
     * Casts the supplied argument to a rvalue reference type of T.
     *
     * @tparam T type of the argument
     * @param value the argument to cast
     * @return an rvalue reference to T
     */
    template <typename T>
    [[nodiscard]] constexpr RemoveReference<T>&& Move(T&& value) noexcept;

    /**
     * Forwards an lvalue as either an lvalue or as an rvalue, depending on T.
     *
     * @tparam T type of the argument reference
     * @param value lvalue to forward
     * @return the forwarded value
     */
    template <typename T>
    [[nodiscard]] constexpr T&& Forward(RemoveReference<T>& value) noexcept;

    /**
     * Forwards rvalues as rvalues and prohibits forwarding of rvalues as lvalues
     *
     * @tparam T type of the argument rvalue
     * @param value rvalue to forward
     * @return the forwarded value
     */
    template <typename T>
    [[nodiscard]] constexpr T&& Forward(T&& value) noexcept;

    /**
     * Represents a global tag that can be used to use a function variant supporting in-place construction.
     */
    struct InPlaceConstructor {
       public:
        constexpr static const InPlaceConstructor* Value = static_cast<const InPlaceConstructor*>(nullptr);

        InPlaceConstructor() = delete;
    };

    /**
     * Indicates whether or not T is one a global Stdlib tag.
     */
    template <typename T>
    constexpr bool IsGlobalTag = IsSame<T, InPlaceConstructor>;

    /**
     * Represents an arbitrary object wrapper that may or may not be initialized.
     *
     * @tparam T type of the underlying object
     */
    template <typename T>
    class Storage {
       public:
        COPYABLE(Storage);
        MOVEABLE(Storage);

        /**
         * Create an empty, un-initialized storage.
         */
        inline Storage() noexcept;

        /**
         * Destructs the contained object if it was initialized.
         */
        ~Storage();

        /**
         * Create a storage based on a copy of the supplied value.
         */
        inline explicit Storage(const T& value);

        /**
         * Create a storage based on a copy of the supplied value.
         */
        inline explicit Storage(T&& value);

        /**
         * Create the Storage object in-place using the supplied arguments
         * @tparam Args arguments to pass to the constructor
         * @param args arguments to pass to the constructor
         */
        template <typename... Args>
        inline explicit Storage(const InPlaceConstructor* /*unused*/, Args&&... args);

        /**
         * Converts this storage to the type of the underlying object
         *
         * @return the underlying object
         */
        [[nodiscard]] inline T& GetObject() noexcept;

        /**
         * Converts this storage to the type of the underlying object
         *
         * @return the underlying object
         */
        [[nodiscard]] inline const T& GetObject() const noexcept;

        /**
         * Returns whether or not this storage was initialized.
         *
         * @return whether or not this storage was initialized.
         */
        [[nodiscard]] bool IsInitialized() const noexcept;

       private:
        void Destroy();

       private:
        uint8_t m_data[sizeof(T)]{0};
        bool m_initialized;
    };

}  // namespace FunnyOS::Stdlib

#include "Utility.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_UTILITY_HPP
