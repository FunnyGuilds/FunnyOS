#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_HPP

#include "System.hpp"
#include "IntegerTypes.hpp"
#include "Functional.hpp"
#include "Memory.hpp"

namespace FunnyOS::Stdlib {

    /**
     * Exception thrown when the accessed index of a vector is greater than the maximum allowed.
     */
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(VectorIndexOutOfBounds);

    /**
     * Thrown when there is not enough memory to allocate or resize vector's heap.
     */
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(VectorNotEnoughMemory);

    /**
     * A dynamic-size container of values stored on heap.
     * The elements are guaranteed to be placed in memory next to each other in an ascending order of indexes.
     * Random access of a vector's element is guaranteed to be O(1)
     *
     * @tparam T type of the elements.
     */
    template <typename T>
    class Vector {
       public:
        /**
         * The default growth factory used by vectors.
         */
        constexpr const static float DEFAULT_GROWTH_FACTOR = 1.5F;

       public:
        /**
         * Type used to iterate over this vector.
         */
        using Iterator = T*;

        /**
         * A const type used to iterate over this vector.
         */
        using ConstIterator = const T*;

       public:
        COPYABLE(Vector);
        MOVEABLE(Vector);

        /**
         * Constructs a new empty vector with the default growth factor.
         */
        Vector();

        /**
         * Constructs a new vector with an initial capacity and the default growth factor.
         *
         * @param[in] initialCapacity initial capacity
         */
        Vector(size_t initialCapacity);

        /**
         * Constructs a new vector with an initial capacity and a specified growth factory.
         *
         * @param[in] initialCapacity initial capacity
         * @param[in] growthFactor growth factor, if there's a need to resize the vector, the new heap's size will be
         * the old size times the growth factor.
         */
        Vector(size_t initialCapacity, float growthFactor);

        /**
         * Constructs a new vector and initializes it with entries from the InitializerList.
         *
         * @param[in] list initializer list
         */
        Vector(InitializerList<T> list);

        /**
         * Destructs the vector and all the elements inside.
         */
        ~Vector();

        /**
         * Gets the size of the vector, the actual number of elements inside it.
         *
         * @return size of the vector
         */
        [[nodiscard]] size_t Size() const noexcept;

        /**
         * Gets the element at N'th index of the vector
         *
         * @param index index
         * @return the N'th element
         *
         * @throws VectorIndexOutOfBounds when index >= GetSize()
         */
        [[nodiscard]] T& operator[](size_t index);

        /**
         * Gets the element at N'th index of the vector
         *
         * @param index index
         * @return  the N'th element
         *
         * @throws VectorIndexOutOfBounds when index >= GetSize()
         */
        [[nodiscard]] const T& operator[](size_t index) const;

        /**
         * Returns the first element of the vector
         *
         * @return the first element of the vector
         *
         * @throws VectorIndexOutOfBounds when vector is empty
         */
        T& Head();

        /**
         * Returns the last element of the vector
         *
         * @return the last element of the vector
         *
         * @throws VectorIndexOutOfBounds when vector is empty
         */
        T& Tail();

        /**
         * Returns the first element of the vector
         *
         * @return the first element of the vector
         *
         * @throws VectorIndexOutOfBounds when vector is empty
         */
        const T& Head() const;

        /**
         * Returns the last element of the vector
         *
         * @return the last element of the vector
         *
         * @throws VectorIndexOutOfBounds when vector is empty
         */
        const T& Tail() const;

        /**
         * Appends a value at the end of the vector.
         *
         * @param value value to append
         */
        void Append(const T& value);

        /**
         * Appends a value at the end of the vector.
         *
         * @param value value to append
         */
        void Append(T&& value);

        /**
         * Create a value in-place at the end of the vector.
         *
         * @param args arguments for the T constructor.
         */
        template <typename... Args>
        void AppendInPlace(Args&&... args);

        /**
         * Removes an element from the specified index.
         *
         * @param index index of the element to remove
         * @throws VectorIndexOutOfBounds when index >= GetSize()
         */
        void Remove(size_t index);

        /**
         * Removes elements starting from the specified index [from] and ending on element with the index [to]
         *
         * @param from first element to remove (inclusive)
         * @param to last element to remove (inclusive)
         *
         * @throws VectorIndexOutOfBounds when index >= GetSize()
         */
        void RemoveRange(size_t from, size_t to);

        /**
         * Resizes the vector's heap to match the size of the vector.
         */
        void ShrinkToSize();

        /**
         * Ensures that the vector's heap is large enough to hold at lesat [num] elements.
         *
         * @param num number of elements that vector's must be able to contain.
         */
        void EnsureCapacity(size_t num);

        /**
         * Returns the capacity of the vector. It is the size of the vector's allocated memory (heap) where all the
         * elements are stored. It is always greater or equal the size of the vector.
         *
         * @return capacity of the vector
         */
        [[nodiscard]] size_t Capacity() const noexcept;

        /**
         * Inserts a value at the specified index. Values at and above and further will be shifted to make room for the
         * inserted value.
         *
         * @param index index where to insert the value at
         * @param value value to insert
         *
         * @throws VectorIndexOutOfBounds when index > GetSize()
         */
        void Insert(size_t index, const T& value);

        /**
         * Inserts a value at the specified index. Values at and above and further will be shifted to make room for the
         * inserted value.
         *
         * @param index index where to insert the value at
         * @param value value to insert
         *
         * @throws VectorIndexOutOfBounds when index > GetSize()
         */
        void Insert(size_t index, T&& value);

        /**
         * Creates a value in-place at the specified index. Values at and above and further will be shifted to make room
         * for the created value.
         *
         * @param index index where to insert the value at
         * @param args arguments for the T constructor.
         *
         * @throws VectorIndexOutOfBounds when index > GetSize()
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
         * @throws VectorIndexOutOfBounds when index > GetSize()
         */
        template <class OtherIterator>
        void Insert(size_t index, OtherIterator value, size_t size);

        /**
         * Removes all elements from this vector.
         */
        void Clear();

        /**
         * Returns an iterator pointing at the beginning of the vector.
         *
         * @return iterator pointing at the beginning of the vector.
         */
        Iterator Begin() noexcept;

        /**
         * Returns an iterator pointing at the end of the vector. (The element 1 after the last element of the vector)
         *
         * @return iterator pointing at the end of the vector.
         */
        Iterator End() noexcept;

        /**
         * Returns an iterator pointing at the beginning of the vector.
         *
         * @return iterator pointing at the beginning of the vector.
         */
        ConstIterator Begin() const noexcept;

        /**
         * Returns an iterator pointing at the end of the vector. (The element 1 after the last element of the vector)
         *
         * @return iterator pointing at the end of the vector.
         */
        ConstIterator End() const noexcept;

        HAS_STANDARD_ITERATORS;

       private:
        void CheckBounds(size_t index) const;

        void Shift(size_t index, size_t count);

        void EnsureCapacityExact(size_t num);

       private:
        float m_growthFactor;
        size_t m_size;
        Memory::SizedBuffer<T> m_data;
    };

}  // namespace FunnyOS::Stdlib

#include "Vector.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_VECTOR_HPP
