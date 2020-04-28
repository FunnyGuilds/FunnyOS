#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP

#include "IntegerTypes.hpp"
#include "Memory.hpp"
#include "Compiler.hpp"
#include "Functional.hpp"

namespace FunnyOS::Stdlib::String {

    /**
     * A SizedBuffer that can hold a string.
     */
    using StringBuffer = Memory::SizedBuffer<char>;

    /**
     * Calculates a length of a string.
     *
     * @param[in] string string to calculate the length of
     *
     * @return length of the string, without the null byte.
     */
    [[nodiscard]] size_t Length(const char* string) noexcept;

    /**
     * Concatenates two strings into an output buffer.
     *
     * @param[out] buffer result buffer
     * @param[in] string1 first string to concatenate
     * @param[in] string2 second string to concatenate
     *
     * @return true if succeeded, false if the buffer was too small
     */
    bool Concat(StringBuffer& buffer, const char* string1, const char* string2) noexcept;

    /**
     * Appends a string at the end of the buffer.
     *
     * @param buffer buffer to append the string to
     * @param string string to append
     * @return true if buffer had enough capacity to contain the string
     */
    bool Append(StringBuffer& buffer, const char* string) noexcept;

    /**
     * Compares up to [length] characters of the string1 to the string2
     * This function starts comparing the first character of each string. If they are equal to each other, it continues
     * with the following pairs until the characters differ, until a terminating null-character is reached, or until num
     * characters match in both strings, whichever happens first.
     *
     * @param[in] string1 string to be compared
     * @param[in] string2 string to be compared
     * @param[in] length maximum number of characters to be compared
     *
     * @return value based on the comparison result
     *         <0 - the first character that does not match has a lower value in string1 than in string 2
     *         0 - strings are identical
     *         >0 - the first character that does not match has a greater value in string1 than in string2
     */
    [[nodiscard]] int Compare(const char* string1, const char* string2, size_t length) noexcept;

    /**
     * Gets the index of the first occurrence of [character] in [string].
     *
     * @param string string to search in
     * @param character character to search for
     * @return index of character, or -1 if not found
     */
    [[nodiscard]] int IndexOf(const char* string, char character);

    /**
     * Equivalent of calling IntegerToString(buffer, integer, 10)
     */
    template <typename Integer>
    bool IntegerToString(StringBuffer& buffer, Integer integer) noexcept;

    /**
     * Converts the given integer to a string using the supplied radix.
     *
     * @tparam Integer type of the int to convert
     * @param[out] buffer output buffer
     * @param[in] integer integer to convert
     * @param[in] radix radix to use
     * @return true if the operation succeeded, false if the radix was invalid or the buffer was to small to hold the
     * result
     */
    template <typename Integer>
    bool IntegerToString(StringBuffer& buffer, Integer integer, uint8_t radix) noexcept;

    /**
     * Converts the given integer to a zero-padded hex string using the supplied radix.
     * The number of zeros to pads will be deduced from Integer type's size
     *
     * @tparam Integer type of the int to convert
     * @param[out] buffer output buffer
     * @param[in] integer integer to convert
     * @return true if the operation succeeded, false if the radix was invalid or the buffer was to small to hold the
     * result
     */
    template <typename Integer>
    bool IntegerToHex(StringBuffer& buffer, Integer integer) noexcept;

    /**
     * Converts the string to an integer.
     *
     * @tparam Integer type of the integer
     * @param buffer string to parse
     * @param radix radix to use
     * @return parsed number or empty optional if the entire string is not a valid number.
     */
    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToInt(const char* buffer, uint8_t radix) noexcept;

    /**
     * Converts the first character of the given string to an integer.
     *
     * @tparam Integer type of the integer
     * @param buffer string to parse
     * @param radix radix to use
     * @return parsed number or empty optional if no number found.
     */
    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToIntLax(const char* buffer, uint8_t radix) noexcept;

    /**
     * Converts the first character of the given string to an integer.
     *
     * @tparam Integer type of the integer
     * @param buffer string to parse
     * @param radix radix to use
     * @param i the current index in the string to use, will be updated to point after the number
     * @return parsed number or empty optional if no number found.
     */
    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToIntLax(const char* buffer, uint8_t radix, size_t& i) noexcept;

    /**
     * Converts the given string to an integer.
     *
     * @tparam Integer type of the integer
     * @param buffer string to parse
     * @param radix radix to use
     * @param strict if true the string must contain only the number, if false only the beginning of the string must be
     * a number
     * @param i the current index in the string to use, will be updated to point after the number
     * @return parsed number or empty optional if fail.
     */
    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToInt(const char* buffer, uint8_t radix, bool strict, size_t& i) noexcept;

    /**
     * Prints formatted data to the output buffer.
     *
     * @param buffer output buffer
     * @param format string format, semi-compatible with cstdlib's printf
     * @param ... additional arguments, depending on format
     *
     * @return true on success, false if the buffer was to small
     */
    bool Format(StringBuffer& buffer, const char* format, ...);

    /**
     * Prints formatted data to the output buffer.
     *
     * @param buffer output buffer
     * @param format string format, semi-compatible with cstdlib's printf
     * @param args additional arguments, depending on format
     *
     * @return true on success, false if the buffer was to small
     */
    bool Format(StringBuffer& buffer, const char* format, va_list* args);

}  // namespace FunnyOS::Stdlib::String

#include "String.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP