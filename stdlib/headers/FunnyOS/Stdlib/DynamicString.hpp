#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_HPP

#include "Dynamic.hpp"
#include "Compiler.hpp"
#include "Functional.hpp"
#include "IntegerTypes.hpp"
#include "Memory.hpp"
#include "Vector.hpp"

namespace FunnyOS::Stdlib {

    /**
     * Exception thrown when the accessed index of a string is greater than the maximum allowed.
     */
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(StringIndexOutOfBounds);

    template <typename T>
    class BasicDynamicString {
       public:
        /**
         * Type used to iterate over this string.
         */
        using Iterator = T*;

        /**
         * A const type used to iterate over this string.
         */
        using ConstIterator = const T*;

       public:
        TRIVIALLY_COPYABLE(BasicDynamicString);
        TRIVIALLY_MOVEABLE(BasicDynamicString);

        /**
         * Constructs a new string that copies all characters from the given string.
         *
         * @param[in] data string to copy characters from, must be terminated by a null byte
         */
        BasicDynamicString(const T* data);

        /**
         * Constructs a new string that copies [length] characters from the given string, starting a [offset]
         *
         * @param[in] data string to copy characters from, must be terminated by a null byte
         * @param offset offset to start copying from
         * @param length number of characters to copy
         */
        BasicDynamicString(const T* data, size_t offset, size_t length);

        /**
         * Constructs a new string that copies all characters from a buffer
         *
         * @param buffer buffer to copy characters from
         */
        BasicDynamicString(const Memory::SizedBuffer<T>& buffer);

       public:
        /**
         * Gets the size of the string, the actual number of characters inside it.
         *
         * @return size of the vector
         */
        [[nodiscard]] size_t Size() const noexcept;

        /**
         * Resizes the string's heap to match the size of the string.
         */
        void ShrinkToSize();

        /**
         * Ensures that the string's heap is large enough to hold at least [num] characters.
         *
         * @param num number of character that string's must be able to contain.
         */
        void EnsureCapacity(size_t num);

        /**
         * Gets the character at N'th index of the string
         *TRIVIALLY_
         * @param index index
         * @return the N'th character
         *
         * @throws StringIndexOutOfBounds when index >= GetSize()
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
         * Appends a string at the end of this string.
         *
         * @param value value to append
         */
        void Append(const BasicDynamicString<T>& value);

        /**
         * Returns an iterator pointing at the beginning of the string.
         *
         * @return iterator pointing at the beginning of the string.
         */
        Iterator Begin() noexcept;

        /**
         * Returns an iterator pointing at the end of the string. (The character 1 after the last element of the string)
         *
         * @return iterator pointing at the end of the string.
         */
        Iterator End() noexcept;

        /**
         * Returns an iterator pointing at the beginning of the string.
         *
         * @return iterator pointing at the beginning of the string.
         */
        ConstIterator Begin() const noexcept;

        /**
         * Returns an iterator pointing at the end of the string. (The character 1 after the last element of the string)
         *
         * @return iterator pointing at the end of the string.
         */
        ConstIterator End() const noexcept;

        HAS_STANDARD_ITERATORS;

       private:
        Stdlib::Vector<T> m_data;
    };

    using DynamicString = BasicDynamicString<char>;

}  // namespace FunnyOS::Stdlib

#include "DynamicString.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMICSTRING_HPP