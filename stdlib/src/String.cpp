#include <FunnyOS/Stdlib/String.hpp>

namespace FunnyOS::Stdlib::String {

    size_t Length(const char* string) {
        size_t length = 0;
        while (*(string + length) != 0) {
            length++;
        }

        return length;
    }

    bool Concat(StringBuffer& buffer, const char* string1, const char* string2) {
        const size_t string1Length = Length(string1);
        const size_t string2Length = Length(string2);

        // Check boundary
        if (buffer.Size < string1Length + string2Length + 1) {
            return false;
        }

        // Copy data
        Memory::Copy(buffer.Data, string1, string1Length);
        Memory::Copy(buffer.Data + string1Length, string2, string2Length);

        // Null byte
        buffer.Data[string1Length + string2Length] = 0;
        return true;
    }

    int StringCompare(const char* string1, const char* string2, size_t length) {
        for (size_t i = 0; i < length; i++) {
            const char c1 = *(string1 + i);
            const char c2 = *(string2 + i);

            // If string ends with a null
            if (c1 == 0) {
                if (c2 == 0) {  // Both end and the same time, they are equal
                    return 0;
                }

                // Otherwise we should return result < 0
                return -c2;
            }

            // c2 has finished, so result > 0, string1 has a bigger value
            if (c2 == 0) {
                return c1;
            }

            const char diff = c1 - c2;

            if (diff != 0) {
                return diff;
            }
        }

        // They are equal
        return 0;
    }

    // NOLINTNEXTLINE(cert-dcl50-cpp)
    bool Format(StringBuffer& buffer, const char* format, ...) {
        va_list args;
        va_start(args, format);
        const bool ret = Format(buffer, format, args);
        va_end(args);
        return ret;
    }

    // NOLINTNEXTLINE(cert-dcl50-cpp)
    bool Format(StringBuffer& buffer, const char* format, va_list args) {
        return String::Concat(buffer, "", format);  // TODO
    }

}  // namespace FunnyOS::Stdlib::String
