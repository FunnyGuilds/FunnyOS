#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP
#error "Include String.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_TCC

namespace FunnyOS::Stdlib::String {

    template <typename Integer>
    bool IntegerToString(StringBuffer& buffer, Integer integer) noexcept {
        return IntegerToString(buffer, integer, 10);
    }

    template <typename Integer>
    bool IntegerToString(StringBuffer& buffer, Integer integer, uint8_t radix) noexcept {
        const char* NUMBER_CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t MAX_RADIX = Length(NUMBER_CHARACTERS);

        if (radix < 2 || radix > MAX_RADIX || buffer.Size == 0) {
            return false;
        }

        // If the number is negative just append '-' at the beginning and convert [-integer] to string.
        if (integer < 0) {
            buffer.Data[0] = '-';
            StringBuffer wrapperBuffer{buffer.Data + 1, buffer.Size - 1};
            return IntegerToString(wrapperBuffer, -integer, radix);
        }

        // Find the highest divisor
        Integer highestDivisor = 1;
        while (integer / highestDivisor >= radix) {
            highestDivisor *= radix;
        }

        // Print the number digit by digit
        size_t i = 0;
        while (highestDivisor != 0) {
            if (i >= buffer.Size) {
                return false;
            }

            buffer.Data[i] = NUMBER_CHARACTERS[integer / highestDivisor];
            integer = integer % highestDivisor;

            highestDivisor /= radix;
            i++;
        }

        if (i >= buffer.Size) {
            // No space for null byte
            return false;
        }

        buffer.Data[i] = 0;
        return true;
    }

    template <typename Integer>
    bool IntegerToHex(StringBuffer& buffer, Integer integer) noexcept {
        const char* NUMBER_CHARACTERS = "0123456789ABCDEF";

        // Check output buffeer size
        const size_t intSize = sizeof(Integer);
        if (buffer.Size < intSize * 2 + 1) {
            return false;
        }

        // Print each byte respectively, one byte = exactly 2 digits in hex
        for (size_t i = 0; i < intSize; i++) {
            const auto shiftBytes = static_cast<uint8_t>(intSize - i - 1);
            const uint8_t part = (integer >> (shiftBytes * 8)) & 0xFF;

            buffer.Data[i * 2 + 0] = NUMBER_CHARACTERS[part >> 4];
            buffer.Data[i * 2 + 1] = NUMBER_CHARACTERS[part & 0x0F];
        }

        // Add null byte
        buffer.Data[intSize * 2] = 0;
        return true;
    }

    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToInt(const char* buffer, uint8_t radix) noexcept {
        size_t i = 0;
        return StringToInt<Integer>(buffer, radix, true, i);
    }

    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToIntLax(const char* buffer, uint8_t radix) noexcept {
        size_t i = 0;
        return StringToInt<Integer>(buffer, radix, false, i);
    }

    template <typename Integer>
    [[nodiscard]] Optional<Integer> StringToIntLax(const char* buffer, uint8_t radix, size_t& i) noexcept {
        return StringToInt<Integer>(buffer, radix, false, i);
    }

    template <typename Integer>
    Optional<Integer> StringToInt(const char* buffer, uint8_t radix, bool strict, size_t& i) noexcept {
        bool inPrefix = true;
        bool hasSign = false;
        bool strictMatch = true;
        bool negative = false;

        uint8_t digits[sizeof(Integer) * 8];
        size_t digitCount = 0;

        for (;;) {
            const char currentCharacter = buffer[i++];

            if (currentCharacter == '\0') {
                break;
            }

            if (inPrefix) {
                if (currentCharacter == ' ') {
                    continue;
                }

                if (!hasSign && currentCharacter == '-') {
                    negative = true;
                    hasSign = true;
                    continue;
                }

                if (!hasSign && currentCharacter == '+') {
                    hasSign = true;
                    continue;
                }

                inPrefix = false;
            }

            int digit = 0;
            if (currentCharacter >= '0' && currentCharacter <= '9') {
                digit = currentCharacter - '0';
            } else if (currentCharacter >= 'A' && currentCharacter <= 'Z') {
                digit = currentCharacter - 'A' + 10;
            } else if (currentCharacter >= 'a' && currentCharacter <= 'z') {
                digit = currentCharacter - 'a' + 10;
            } else {
                strictMatch = false;
                break;
            }

            if (digit >= radix) {
                strictMatch = false;
                break;
            }

            digits[digitCount++] = digit;
        }
        i--;

        if (digitCount == 0) {
            return EmptyOptional<Integer>();
        }

        if (strict && !strictMatch) {
            return EmptyOptional<Integer>();
        }

        Integer currentValue = 0;
        Integer currentMultiplier = 1;
        for (ssize_t d = digitCount - 1; d >= 0; d--) {
            currentValue += digits[d] * currentMultiplier;
            currentMultiplier *= radix;
        }
        if (negative) {
            currentValue = -currentValue;
        }

        return currentValue;
    }
}  // namespace FunnyOS::Stdlib::String

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP
