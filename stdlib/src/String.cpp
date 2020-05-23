#include <FunnyOS/Stdlib/String.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>

namespace FunnyOS::Stdlib::String {

    size_t Length(const char* string) noexcept {
        size_t length = 0;
        while (*(string + length) != 0) {
            length++;
        }

        return length;
    }

    bool Concat(StringBuffer& buffer, const char* string1, const char* string2) noexcept {
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

    bool Append(StringBuffer& buffer, const char* string) noexcept {
        const size_t bufferStringLength = Length(buffer.Data);
        const size_t appendStringLength = Length(string);

        if (buffer.Size < bufferStringLength + appendStringLength + 1) {
            return false;
        }

        Memory::Copy(buffer.Data + bufferStringLength, string, appendStringLength);
        buffer.Data[bufferStringLength + appendStringLength] = 0;
        return true;
    }

    int Compare(const char* string1, const char* string2, size_t length) noexcept {
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

            const char diff = c1 - c2;  // NOLINT(bugprone-narrowing-conversions)

            if (diff != 0) {
                return diff;
            }
        }

        // They are equal
        return 0;
    }

    int IndexOf(const char* string, char character) {
        int i = 0;

        while (string[i] != 0) {
            if (string[i] == character) {
                return i;
            }

            i++;
        }

        return -1;
    }

    // NOLINTNEXTLINE(cert-dcl50-cpp)
    bool Format(StringBuffer& buffer, const char* format, ...) {
        va_list args;
        va_start(args, format);
        const bool ret = Format(buffer, format, &args);
        va_end(args);
        return ret;
    }

    namespace {
        enum class ParsePhase { Flags, Width, Length, Specifier };

        namespace FormatFlags {
            const int LeftJustify = 1 << 0;
            const int ForceSignCharacter = 1 << 1;
            const int SpaceIfNoSign = 1 << 2;
            const int ForcePrefixOrSuffix = 1 << 3;
            const int PadWithZeroes = 1 << 4;
        }  // namespace FormatFlags

        const char* VALID_FLAGS = "-+ #0";

        enum class FormatLength { Int8, Int16, Int32, Int64, IntMax, SizeT, Ptrdiff };
        const char* VALID_LENGTHS = "hljzt";

        enum class SpecifierType {
            SignedDecimalInteger1 = 'd',
            SignedDecimalInteger2 = 'i',
            UnsignedDecimalInteger = 'u',
            UnsignedOctal = 'o',
            UnsignedHexadecimalIntegerLowercase = 'x',
            UnsignedHexadecimalIntegerUppercase = 'X',
            BinaryInteger = 'b',
            Character = 'c',
            String = 's',
            Invalid = 0
        };
        const char* VALID_SPECIFIERS = "diuoxXbcs";

        struct FormatSpecifier {
            int Flags;
            int Width;
            FormatLength Length;
            SpecifierType Type;
        };

        bool FormatTryAppend(StringBuffer& buffer, size_t& bufferCharacter, const char character) {
            if (bufferCharacter >= buffer.Size) {
                buffer.Data[buffer.Size - 1] = 0;
                return false;
            }

            buffer.Data[bufferCharacter++] = character;
            return true;
        }

        FormatSpecifier ParseFormat(const char* format, size_t& character) {
            FormatSpecifier out{};
            out.Type = SpecifierType::Invalid;
            out.Length = FormatLength::Int32;

            ParsePhase phase = ParsePhase::Flags;
            char currentCharacter;

            while ((currentCharacter = *(format + character)) != 0) {
                switch (phase) {
                    case ParsePhase::Flags: {
                        const int index = IndexOf(VALID_FLAGS, currentCharacter);

                        if (index == -1) {
                            phase = ParsePhase::Width;
                            continue;
                        }

                        out.Flags |= 1 << index;
                        character++;
                        break;
                    }
                    case ParsePhase::Width: {
                        out.Width = StringToInt<int>(format, 10, false, character).GetValueOrDefault(0);
                        phase = ParsePhase::Length;
                        break;
                    }
                    case ParsePhase::Length: {
                        if (IndexOf(VALID_LENGTHS, currentCharacter) == -1) {
                            phase = ParsePhase::Specifier;
                            continue;
                        }

                        if (currentCharacter == 'h') {
                            if (out.Length == FormatLength::Int16) {
                                out.Length = FormatLength::Int8;
                            } else {
                                out.Length = FormatLength::Int16;
                            }
                        } else if (currentCharacter == 'l') {
                            out.Length = FormatLength::Int64;
                        } else if (currentCharacter == 'j') {
                            out.Length = FormatLength::IntMax;
                        } else if (currentCharacter == 'z') {
                            out.Length = FormatLength::SizeT;
                        } else if (currentCharacter == 't') {
                            out.Length = FormatLength::Ptrdiff;
                        }

                        character++;
                        break;
                    }
                    case ParsePhase::Specifier: {
                        if (IndexOf(VALID_SPECIFIERS, currentCharacter) == -1) {
                            return out;
                        }

                        out.Type = static_cast<SpecifierType>(currentCharacter);
                        character++;
                        return out;
                    }
                }
            }

            return out;
        }

        template <typename Integer>
        bool FormatProcessSigned(StringBuffer& buffer, Integer integer, uint8_t radix, bool& isNegative) noexcept {
            static_assert(NumeralTraits::Info<Integer>::IsSigned);

            if (integer < 0) {
                isNegative = true;
                integer = -integer;
            }

            return IntegerToString<Integer>(buffer, integer, radix);
        }

        void FormatTryWriteNumeral(StringBuffer buffer, FormatSpecifier specifier, bool& isNegative, va_list* list) {
            bool isSigned = specifier.Type == SpecifierType::SignedDecimalInteger1 ||
                            specifier.Type == SpecifierType::SignedDecimalInteger2;

            size_t radix;

            switch (specifier.Type) {
                case SpecifierType::UnsignedHexadecimalIntegerLowercase:
                case SpecifierType::UnsignedHexadecimalIntegerUppercase:
                    radix = 16;
                    break;
                case SpecifierType::UnsignedOctal:
                    radix = 8;
                    break;
                case SpecifierType::BinaryInteger:
                    radix = 2;
                    break;
                default:
                    radix = 10;
                    break;
            }

            if (isSigned) {
                if (specifier.Length == FormatLength::Int8) {
                    FormatProcessSigned<int8_t>(buffer, va_arg(*list, int), radix, isNegative);
                } else if (specifier.Length == FormatLength::Int16) {
                    FormatProcessSigned<int16_t>(buffer, va_arg(*list, int), radix, isNegative);
                } else if (specifier.Length == FormatLength::Int32) {
                    FormatProcessSigned<int32_t>(buffer, va_arg(*list, int32_t), radix, isNegative);
                } else if (specifier.Length == FormatLength::Int64) {
                    FormatProcessSigned<int64_t>(buffer, va_arg(*list, int64_t), radix, isNegative);
                } else if (specifier.Length == FormatLength::IntMax) {
                    FormatProcessSigned<intmax_t>(buffer, va_arg(*list, intmax_t), radix, isNegative);
                } else if (specifier.Length == FormatLength::SizeT) {
                    FormatProcessSigned<ssize_t>(buffer, va_arg(*list, ssize_t), radix, isNegative);
                } else if (specifier.Length == FormatLength::Ptrdiff) {
                    FormatProcessSigned<ptrdiff_t>(buffer, va_arg(*list, ptrdiff_t), radix, isNegative);
                }
            } else {
                if (specifier.Length == FormatLength::Int8) {
                    IntegerToString<uint8_t>(buffer, va_arg(*list, unsigned int), radix);
                } else if (specifier.Length == FormatLength::Int16) {
                    IntegerToString<uint16_t>(buffer, va_arg(*list, unsigned int), radix);
                } else if (specifier.Length == FormatLength::Int32) {
                    IntegerToString<uint32_t>(buffer, va_arg(*list, uint32_t), radix);
                } else if (specifier.Length == FormatLength::Int64) {
                    IntegerToString<uint64_t>(buffer, va_arg(*list, uint64_t), radix);
                } else if (specifier.Length == FormatLength::IntMax) {
                    IntegerToString<uintmax_t>(buffer, va_arg(*list, uintmax_t), radix);
                } else if (specifier.Length == FormatLength::SizeT) {
                    IntegerToString<size_t>(buffer, va_arg(*list, size_t), radix);
                } else if (specifier.Length == FormatLength::Ptrdiff) {
                    IntegerToString<ptrdiff_t>(buffer, va_arg(*list, ptrdiff_t), radix);
                }
            }

            if (specifier.Type == SpecifierType::UnsignedHexadecimalIntegerLowercase) {
                size_t i = 0;
                char current;

                while (i < buffer.Size && (current = buffer.Data[i]) != 0) {
                    if (current >= 'A' && current <= 'F') {
                        buffer.Data[i] = (current - 'A' + 'a');
                    }

                    i++;
                }
            }
        }
    }  // namespace

    // NOLINTNEXTLINE(cert-dcl50-cpp)
    bool Format(StringBuffer& buffer, const char* format, va_list* args) {
        char value[256];
        StringBuffer valueBuffer = {value, 255};
        char prefix[256];
        StringBuffer prefixBuffer = {prefix, 255};

        size_t bufferCharacter = 0;
        size_t formatCharacter = 0;
        char currentCharacter;

        while ((currentCharacter = *(format + formatCharacter++)) != 0) {
            if (currentCharacter != '%') {
                if (!FormatTryAppend(buffer, bufferCharacter, currentCharacter)) {
                    return false;
                }

                continue;
            }

            // If it is '%%' we just append a single '%' sign
            if (*(format + formatCharacter) == '%') {
                if (!FormatTryAppend(buffer, bufferCharacter, currentCharacter)) {
                    return false;
                }

                continue;
            }

            FormatSpecifier specifier = ParseFormat(format, formatCharacter);

            if (specifier.Type == SpecifierType::Invalid) {
                continue;
            }

            bool isNegative = false;

            if (specifier.Type == SpecifierType::Character) {
                value[0] = va_arg(*args, int);
                value[1] = 0;
            } else if (specifier.Type == SpecifierType::String) {
                const char* ptr = va_arg(*args, const char*);
                const auto len = Min<size_t>(Length(ptr), sizeof(value) / sizeof(char) - 1);
                Memory::Copy(value, ptr, len);
                value[len] = 0;
            } else {
                FormatTryWriteNumeral(valueBuffer, specifier, isNegative, args);
            }

            if (isNegative) {
                prefix[0] = '-';
            } else if ((specifier.Flags & FormatFlags::ForceSignCharacter) != 0) {
                prefix[0] = '+';
            } else if ((specifier.Flags & FormatFlags::SpaceIfNoSign) != 0) {
                prefix[0] = ' ';
            } else {
                prefix[0] = 0;
            }
            prefix[1] = 0;

            if ((specifier.Flags & FormatFlags::ForcePrefixOrSuffix) != 0) {
                if (specifier.Type == SpecifierType::UnsignedHexadecimalIntegerUppercase) {
                    String::Concat(prefixBuffer, prefix, "0X");
                } else if (specifier.Type == SpecifierType::UnsignedHexadecimalIntegerLowercase) {
                    String::Concat(prefixBuffer, prefix, "0x");
                } else if (specifier.Type == SpecifierType::UnsignedOctal) {
                    String::Concat(prefixBuffer, prefix, "0");
                }
            }

            const size_t totalSize = Length(value);
            const size_t prefixSize = Length(prefix);
            const auto padSize = static_cast<size_t>(Max<ssize_t>(0, specifier.Width - totalSize - prefixSize));
            const char padCharacter = (specifier.Flags & FormatFlags::PadWithZeroes) != 0 ? '0' : ' ';
            const bool padLeft = padCharacter == '0' || (specifier.Flags & FormatFlags::LeftJustify) != 0;

            if (padLeft && padCharacter == ' ') {
                for (size_t i = 0; i < padSize; i++) {
                    if (!FormatTryAppend(buffer, bufferCharacter, padCharacter)) {
                        return false;
                    }
                }
            }

            for (size_t i = 0; i < prefixSize; i++) {
                if (!FormatTryAppend(buffer, bufferCharacter, prefix[i])) {
                    return false;
                }
            }

            if (padLeft && padCharacter == '0') {
                for (size_t i = 0; i < padSize; i++) {
                    if (!FormatTryAppend(buffer, bufferCharacter, padCharacter)) {
                        return false;
                    }
                }
            }

            for (size_t i = 0; i < totalSize; i++) {
                if (!FormatTryAppend(buffer, bufferCharacter, value[i])) {
                    return false;
                }
            }

            if (!padLeft) {
                for (size_t i = 0; i < padSize; i++) {
                    if (!FormatTryAppend(buffer, bufferCharacter, padCharacter)) {
                        return false;
                    }
                }
            }
        }

        return FormatTryAppend(buffer, bufferCharacter, 0);
    }

}  // namespace FunnyOS::Stdlib::String
