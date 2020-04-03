#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP
#error "Include String.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_TCC

namespace FunnyOS::Stdlib::String {

    template <typename Integer> bool IntegerToString(StringBuffer& buffer, Integer integer) {
        return IntegerToString(buffer, integer, 10);
    }

    template <typename Integer> bool IntegerToString(StringBuffer& buffer, Integer integer, uint8_t radix) {
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

    template <typename Integer> bool IntegerToHex(StringBuffer& buffer, Integer integer) {
        const char* NUMBER_CHARACTERS = "0123456789ABCDEF";

        // Check output buffeer size
        const size_t intSize = sizeof(Integer);
        if (buffer.Size < intSize * 2 + 1) {
            return false;
        }

        // Print each byte respectively, one byte = exactly 2 digits in hex
        for (size_t i = 0 ; i < intSize ; i++) {
            const uint8_t shiftBytes = static_cast<const uint8_t>(intSize - i - 1);
            const uint8_t part = (integer >> (shiftBytes * 8)) & 0xFF;

            buffer.Data[i * 2 + 0] = NUMBER_CHARACTERS[part >> 4];
            buffer.Data[i * 2 + 1] = NUMBER_CHARACTERS[part & 0x0F];
        }

        // Add null byte
        buffer.Data[intSize * 2] = 0;
        return true;
    }
}  // namespace FunnyOS::Stdlib::String

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STRING_HPP
