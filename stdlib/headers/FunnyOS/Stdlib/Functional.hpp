#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP

#include "System.hpp"
#include "Utility.hpp"

namespace FunnyOS::Stdlib {

    struct InPlaceConstructor {
        constexpr static const InPlaceConstructor* Value = static_cast<const InPlaceConstructor*>(nullptr);
    };

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
        [[nodiscard]] inline T& GetObject();

        /**
         * Converts this storage to the type of the underlying object
         *
         * @return the underlying object
         */
        [[nodiscard]] inline const T& GetObject() const;

        /**
         * Returns whether or not this storage was initialized.
         *
         * @return whether or not this storage was initialized.
         */
        [[nodiscard]] bool IsInitialized() const;

       private:
        void Destroy();

       private:
        uint8_t m_data[sizeof(T)]{0};
        bool m_initialized;
    };

    /**
     * Thrown when an empty optional is accessed improperly.
     */
    F_TRIVIAL_EXCEPTION(EmptyOptionalException, "FunnyOS::Stdlib::EmptyOptionalException");

    /**
     * Represents a type that can be passed to an optional constructor to create an empty optional.
     */
    struct NullOptionalType {
        constexpr static const NullOptionalType* Value = static_cast<const NullOptionalType*>(nullptr);
    };

    /**
     * Represents a container that can have a value or be empty.
     *
     * @tparam T
     */
    template <typename T>
    class Optional {
       public:
        TRIVIALLY_COPYABLE(Optional);
        TRIVIALLY_MOVEABLE(Optional);

        /**
         * Constructs an empty optional.
         */
        inline Optional() noexcept;

        /**
         * Destructs the optional
         */
        ~Optional() = default;

        /**
         * Constructs an empty optional
         */
        inline explicit Optional(const NullOptionalType* /*unused*/) noexcept;

        /**
         * Constructs an optional and initializes it using the provided value.
         */
        template <typename U = T, typename = EnableIf<IsSame<RemoveReference<T>, RemoveReference<U>>>>  // TODO
        inline Optional(U&& value);

        /**
         * Returns whether or not this optional has a value
         * @return whether or not this optional has a value
         */
        [[nodiscard]] inline bool HasValue() const noexcept;

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline T& GetValue();

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline const T& GetValue() const;

        /**
         * Returns the value of this optional or [defaultValue] if te optional is not initialized.
         *
         * @param defaultValue default value
         * @return the value of this optional or [defaultValue]
         */
        inline T GetValueOrDefault(T&& defaultValue) noexcept;

        /**
         * Returns the value of this optional or [defaultValue] if te optional is not initialized.
         *
         * @param defaultValue default value
         * @return the value of this optional or [defaultValue]
         */
        inline T GetValueOrDefault(T&& defaultValue) const noexcept;

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline operator T&();

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline operator const T&() const;

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline operator bool();

        /**
         * Returns the value of this optional or throws EmptyOptionalException if te optional is not initialized.
         *
         * @return the value of this optional
         */
        [[nodiscard]] inline operator bool() const;

       private:
        template <typename T2, typename... Args>
        friend Optional<T2> MakeOptional(Args&&... args);

        template <typename... Args>
        inline explicit Optional(const InPlaceConstructor*, Args&&... args);

       private:
        Storage<T> m_storage;
    };

    /**
     * An empty optional of type T
     *
     * @tparam T type of the optional
     */
    template <typename T>
    [[nodiscard]] inline Optional<T> EmptyOptional() noexcept;

    /**
     * Makes an Optional with its value in-place.
     *
     * @tparam T type of the value
     * @tparam Args args to be passed to the constructor of T
     * @param args args to be passed to the constructor of T
     * @return the newly created Optional
     */
    template <typename T, typename... Args>
    [[nodiscard]] inline Optional<T> MakeOptional(Args&&... args);

}  // namespace FunnyOS::Stdlib

#include "Functional.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FUNCTIONAL_HPP
