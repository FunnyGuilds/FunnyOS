#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP

#include "Memory.hpp"

namespace FunnyOS::Stdlib {

    namespace _Internal {
        template <typename T>
        struct DefaultDeleter;
    }  // namespace _Internal

    /**
     * A SizedBuffer wrapper that will automatically allocate data on construction and free on destruction.
     *
     * All Memory::*Buffer functions are still valid on a SmartSizedBuffer.
     *
     * @tparam T pointer type
     */
    template <typename T>
    struct SmartSizedBuffer : public Memory::SizedBuffer<T> {
       public:
        COPYABLE(SmartSizedBuffer);
        MOVEABLE(SmartSizedBuffer);

        /**
         * Consrtucts the buffer and allocates [size] * sizeof(T) bytes of data.
         *
         * @param size size of the buffer
         */
        SmartSizedBuffer(size_t size);

        /**
         * Destructs the buffer and frees its data.
         */
        virtual ~SmartSizedBuffer();
    };

    /**
     * A smart pointer that is the sole owner of the contained object.
     * Owner deletes the contained object when it goes out of scope.
     *
     * @tparam T type of the owned element.
     * @tparam Deleter functor used to delete object contained in Owner
     */
    template <typename T, typename Deleter = _Internal::DefaultDeleter<T>>
    class Owner {
       public:
        NON_COPYABLE(Owner);
        MOVEABLE(Owner);

        /**
         * Constructs a new Owner that does not own anything.
         */
        Owner();

        /**
         * Constructs a new Owner that owns [ptr]
         */
        Owner(T* ptr);

        /**
         * Destructs the Owner and the object that is owns.
         */
        virtual ~Owner();

        /**
         * Destructs the object owned by this Owner and take the ownership of [ptr].
         *
         * @param ptr new object to take the ownership of (may be nullptr).
         */
        void Reset(T* ptr) noexcept;

        /**
         * Returns the pointer to the owned object and releases the ownership of it.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        T* Release() noexcept;

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        T* Get() noexcept;

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        const T* Get() const noexcept;

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        T* operator->();

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        const T* operator->() const;

       private:
        Deleter m_deleter{};
        T* m_data;
    };

    /**
     * Constructs an object of type T and an Owner<T, Deleter> that owns T.
     *
     * @tparam T type of the object to create
     * @tparam Args type of the arguments used to create instance of [T]
     * @tparam Deleter deleter used to delete T
     * @param args args to create an instance of [T]
     * @return the newly created Owner owning the newly created [T] instance.
     */
    template <typename T, typename... Args, typename Deleter = _Internal::DefaultDeleter<T>>
    Owner<T, Deleter> MakeOwner(Args&&... args);

    namespace _Internal {
        /**
         * Reference counter helper.
         */
        class RefCounter {
           public:
            /**
             * Constructs an uninitialized RefCounter.
             */
            inline RefCounter();

            /**
             * Initializes the ref counter and sets it counter to [1]
             */
            inline void Initialize();

            /**
             * If counter is initialized it is incremented by [1].
             *
             * @return copy of this counter.
             */
            inline RefCounter Acquire();

            /**
             * If counter is initialized it is decremented by [1].
             *
             * @return if this operation set the counter value to [0].
             */
            inline bool Release();

            /**
             * Invalidates this ref counter and makes it uninitialized.
             */
            inline void Invalidate();

           private:
            unsigned int* m_counter;
        };
    }  // namespace _Internal

    /**
     * A reference counting smart pointer. Multiple Refs may own the same object.
     *
     * When Ref goes out of scope, it is resettted or re-assigned it will delete the owned object ONLY if that Ref is
     * the only owner of T.
     *
     * @tparam T type of the owned element
     */
    template <typename T>
    class Ref {
       public:
        // TODO: Custom deleter, unit tests
        COPYABLE(Ref);
        MOVEABLE(Ref);

        /**
         * Constructs a new Ref that does not own anything.
         */
        Ref();

        /**
         * Constructs a new Ref that is the new sole owner of [ptr].
         */
        Ref(T* ptr);

        /**
         * Destructs the Ref.
         *
         * If this Ref is the last owner of the owned object, the owned object is also destroyed.
         */
        virtual ~Ref();

        /**
         * If this Ref is the last owner of the owned object, the owned object is destroyed.
         * This Ref then takes ownership of [data].
         *
         * @param data new object to take the ownership of (may be nullptr, then now ownership is taken)
         */
        void Reset(T* data) noexcept;

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        T* Get() const noexcept;

        /**
         * Returns the pointer to the owned object.
         *
         * @return the owned object (may be nullptr if the Owner did not own anything)
         */
        T* operator->() const noexcept;

       private:
        template <typename U>
        Ref(const Ref<U>& ref, T* ptr);

        template <typename U>
        Ref(Ref<U>&& ref, T* ptr);

       private:
        mutable _Internal::RefCounter m_refCounter;
        T* m_data;

       private:
        template <typename U>
        friend class Ref;

        template <typename T2, typename U2>
        friend Ref<T2> StaticRefCast(const Ref<U2>& ref);

        template <typename T2, typename U2>
        friend Ref<T2> StaticRefCast(Ref<U2>&& ref);
    };

    /**
     * Constructs an object of type T and an Ref<T> that owns T.
     *
     * @tparam T type of the object to create
     * @tparam Args type of the arguments used to create instance of [T]
     * @tparam Deleter deleter used to delete T
     * @param args args to create an instance of [T]
     * @return the newly created Ref owning the newly created [T] instance.
     */
    template <typename T, typename... Args>
    Ref<T> MakeRef(Args&&... args);

    /**
     * Creates a new instance of Ref<T> that owns the same object as [ref].
     * Conversion from U* to T* is done via [ static_cast<T*>(ref.Get()) ].
     *
     * @param ref Ref to convert
     * @return the converted ref.
     */
    template <typename T, typename U>
    Ref<T> StaticRefCast(const Ref<U>& ref);

    /**
     * Creates a new instance of Ref<T> that owns the same object as [ref].
     * Conversion from U* to T* is done via [ static_cast<T*>(ref.Get()) ].
     * After calling this function [ref] is no longer in a valid state and should not be used.
     *
     * @param ref Ref to convert
     * @return the converted ref.
     */
    template <typename T, typename U>
    Ref<T> StaticRefCast(Ref<U>&& ref);

}  // namespace FunnyOS::Stdlib

#include "Dynamic.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
