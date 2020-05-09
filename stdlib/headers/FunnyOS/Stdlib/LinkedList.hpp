#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_HPP

#include "System.hpp"
#include "IntegerTypes.hpp"
#include "Functional.hpp"

namespace FunnyOS::Stdlib {

    /**
     * Exception thrown when the accessed index of a linked list is greater than the maximum allowed.
     */
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(LinkedListIndexOutOfBounds);

    /**
     * A dynamic-size container of linked values.
     * The elements are placed randomly on the heap and each element points to the next one.
     *
     * Inserting at the end or beginning of the list is guaranteed to be O(1).
     *
     * @tparam T type of the elements.
     */
    template <typename T>
    class LinkedList {
       private:
        struct Element;  // Forward declaration

       public:
        /**
         * A const type used to iterate over this list.
         */
        class ConstIterator {
           public:
            TRIVIALLY_COPYABLE(ConstIterator);
            TRIVIALLY_MOVEABLE(ConstIterator);

            /**
             * Increment this iterator by one.
             *
             * @return *this
             */
            ConstIterator operator++() noexcept;

            /**
             * Increment this iterator by one.
             *
             * @return *this
             */
            ConstIterator operator++(int) noexcept;

            /**
             * Destructs the iterator, this has no effect on the list.
             */
            ~ConstIterator() = default;

            /**
             * Gets the value that this iterator is pointing at the moment.
             *
             * @return value that this iterator is pointing at the moment.
             */
            const T& operator*() const noexcept;

            /**
             * Gets a pointer to the value that this iterator is pointing at the moment.
             *
             * @return a pointer to the value that this iterator is pointing at the moment.
             */
            const T* operator->() const noexcept;

            /**
             * Checks whether or not the [other] iterator is pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is pointing at the same element
             */
            bool operator==(const ConstIterator& other) const noexcept;

            /**
             * Checks whether or not the [other] iterator is not pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is not pointing at the same element
             */
            bool operator!=(const ConstIterator& other) const noexcept;

            /**
             * Checks whether or not the [other] iterator is pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is pointing at the same element
             */
            bool operator==(ConstIterator& other) noexcept;

            /**
             * Checks whether or not the [other] iterator is not pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is not pointing at the same element
             */
            bool operator!=(ConstIterator& other) noexcept;

           protected:
            ConstIterator(Element* element);

            friend class LinkedList;

            Element* GetElement() const;

           private:
            Element* m_element;
        };

        /**
         * Type used to iterate over this list.
         */
        class Iterator : public ConstIterator {
           public:
            TRIVIALLY_COPYABLE(Iterator);
            TRIVIALLY_MOVEABLE(Iterator);

            /**
             * Destructs the iterator, this has no effect on the list.
             */
            ~Iterator() = default;

            /**
             * Gets the value that this iterator is pointing at the moment.
             *
             * @return value that this iterator is pointing at the moment.
             */
            T& operator*() noexcept;

            /**
             * Gets a pointer to the value that this iterator is pointing at the moment.
             *
             * @return a pointer to the value that this iterator is pointing at the moment.
             */
            T* operator->() noexcept;

           protected:
            Iterator(Element* element);

            friend class LinkedList;
        };

       public:
        COPYABLE(LinkedList);
        MOVEABLE(LinkedList);

        /**
         * Constructs a new empty linked list with no elements.
         */
        LinkedList();

        /**
         * Constructs a new linked list and initializes it with entries from the InitializerList.
         *
         * @param[in] list initializer list
         */
        LinkedList(InitializerList<T> list);

        /**
         * Destructs the list and all the elements inside.
         */
        ~LinkedList();

        /**
         * Gets the size of the linked list, the actual number of elements inside it.
         *
         * @return size of the list
         */
        [[nodiscard]] size_t Size() const noexcept;

        /**
         * Gets the element at N'th index of the list
         *
         * @param index index
         * @return  the N'th element
         *
         * @throws LinkedListIndexOutOfBounds when index >= GetSize()
         */
        [[nodiscard]] T& operator[](size_t index);

        /**
         * Gets the element at N'th index of the list
         *
         * @param index index
         * @return  the N'th element
         *
         * @throws LinkedListIndexOutOfBounds when index >= GetSize()
         */
        [[nodiscard]] const T& operator[](size_t index) const;

        /**
         * Returns the first element of the list
         *
         * @return the first element of the list
         *
         * @throws LinkedListIndexOutOfBounds when list is empty
         */
        T& Head();

        /**
         * Returns the last element of the list
         *
         * @return the last element of the list
         *
         * @throws LinkedListIndexOutOfBounds when list is empty
         */
        T& Tail();

        /**
         * Returns the first element of the list
         *
         * @return the first element of the list
         *
         * @throws LinkedListIndexOutOfBounds when list is empty
         */
        const T& Head() const;

        /**
         * Returns the last element of the list
         *
         * @return the last element of the list
         *
         * @throws LinkedListIndexOutOfBounds when list is empty
         */
        const T& Tail() const;

        /**
         * Appends a value at the end of the list.
         *
         * @param value value to append
         */
        void Append(const T& value);

        /**
         * Appends a value at the end of the list.
         *
         * @param value value to append
         */
        void Append(T&& value);

        /**
         * Create a value in-place at the end of the list.
         *
         * @param args arguments for the T constructor.
         */
        template <typename... Args>
        void AppendInPlace(Args&&... args);

        /**
         * Removes an element from the specified index.
         *
         * @param index index of the element to remove
         * @throws LinkedListIndexOutOfBounds when index >= GetSize()
         */
        void Remove(size_t index);

        /**
         * Removes elements starting from the specified index [from] and ending on element with the index [to]
         *
         * @param from first element to remove (inclusive)
         * @param to last element to remove (inclusive)
         *
         * @throws LinkedListIndexOutOfBounds when index >= GetSize()
         */
        void RemoveRange(size_t from, size_t to);

        /**
         * Inserts a value at the specified index. Values at and above and further will be shifted to make room for the
         * inserted value.
         *
         * @param index index where to insert the value at
         * @param value value to insert
         *
         * @throws LinkedListIndexOutOfBounds when index > GetSize()
         */
        void Insert(size_t index, const T& value);

        /**
         * Inserts a value at the specified index. Values at and above and further will be shifted to make room for the
         * inserted value.
         *
         * @param index index where to insert the value at
         * @param value value to insert
         *
         * @throws LinkedListIndexOutOfBounds when index > GetSize()
         */
        void Insert(size_t index, T&& value);

        /**
         * Creates a value in-place at the specified index. Values at and above and further will be shifted to make room
         * for the created value.
         *
         * @param index index where to insert the value at
         * @param args arguments for the T constructor.
         *
         * @throws LinkedListIndexOutOfBounds when index > GetSize()
         */
        template <typename... Args>
        void InsertInPlace(size_t index, Args&&... value);

        /**
         * Inserts [size] values at the specified index. Values at and above and further will be shifted to make room
         * for the inserted values.
         *
         * @param index index where to insert the first element at.
         * @param value iterator pointing at the start of elements to insert
         * @param size how many elements to insert
         *
         * @throws LinkedListIndexOutOfBounds when index > GetSize()
         */
        template <typename OtherIterator>
        void Insert(size_t index, OtherIterator value, size_t size);

        /**
         * Removes all elements from this list.
         */
        void Clear();

        /**
         * Returns an iterator pointing at the beginning of the list.
         *
         * @return iterator pointing at the beginning of the list.
         */
        Iterator Begin() noexcept;

        /**
         * Returns an iterator pointing at the end of the list. (The element 1 after the last element of the list)
         *
         * @return iterator pointing at the end of the list.
         */
        Iterator End() noexcept;

        /**
         * Returns an iterator pointing at the beginning of the list.
         *
         * @return iterator pointing at the beginning of the list.
         */
        ConstIterator Begin() const noexcept;

        /**
         * Returns an iterator pointing at the end of the list. (The element 1 after the last element of the list)
         *
         * @return iterator pointing at the end of the list.
         */
        ConstIterator End() const noexcept;

        HAS_STANDARD_ITERATORS;

       private:
        struct Element {
            Storage<T> Data;
            Element* Previous;
            Element* Next;
        };

       private:
        void CheckBounds(size_t num) const;

        Element* ElementAt(size_t num) const;

        void InsertElement(size_t index, Element* element);

        void RemoveElement(Element* element);

       private:
        size_t m_size{0};
        Element* m_head;
        Element* m_tail;
    };
}  // namespace FunnyOS::Stdlib

#include "LinkedList.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_HPP