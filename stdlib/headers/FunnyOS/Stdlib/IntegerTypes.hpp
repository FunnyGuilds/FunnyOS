#ifndef FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#define FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#include "TypeTraits.hpp"
#include "IntegerTypes.tcc"

namespace FunnyOS::Stdlib {
    // clang-format off
    using  int8_t       =           char;                     // NOLINT(google-runtime-int)
    using  int16_t      =           short;                    // NOLINT(google-runtime-int)
    using  int32_t      =           int;                      // NOLINT(google-runtime-int)
    using uint8_t       =  unsigned char;                     // NOLINT(google-runtime-int)
    using uint16_t      =  unsigned short;                    // NOLINT(google-runtime-int)
    using uint32_t      =  unsigned int;                      // NOLINT(google-runtime-int)

    #ifdef F_64
        using  int64_t      =           long int;             // NOLINT(google-runtime-int)
        using uint64_t      =  unsigned long int;             // NOLINT(google-runtime-int)
    #else
        using  int64_t      =           long long int;        // NOLINT(google-runtime-int)
        using uint64_t      =  unsigned long long int;        // NOLINT(google-runtime-int)
    #endif

    static_assert(sizeof(int8_t  ) == 8  / 8, "Invalid int8_t size"  );
    static_assert(sizeof(int16_t ) == 16 / 8, "Invalid int16_t size" );
    static_assert(sizeof(int32_t ) == 32 / 8, "Invalid int32_t size" );
    static_assert(sizeof(int64_t ) == 64 / 8, "Invalid int64_t size" );
    static_assert(sizeof(uint8_t ) == 8  / 8, "Invalid uint8_t size" );
    static_assert(sizeof(uint16_t) == 16 / 8, "Invalid uint16_t size");
    static_assert(sizeof(uint32_t) == 32 / 8, "Invalid uint32_t size");
    static_assert(sizeof(uint64_t) == 64 / 8, "Invalid uint64_t size");

    #ifdef F_64
       using intmax_t   =  int64_t;
       using uintmax_t  = uint64_t;
    #else
        using  intmax_t =  int32_t;
        using uintmax_t = uint32_t;
    #endif

    using  size_t    =  uintmax_t;
    using ssize_t    =   intmax_t;

    using  intptr_t   =  intmax_t;
    using uintptr_t   = uintmax_t;
    using ptrdiff_t  = intptr_t;


    namespace Detail {
        template <typename T> struct ToUnsigned {};
        template <typename T> struct ToSigned {};

        template<> struct ToUnsigned< int8_t > : HasType<uint8_t > {};
        template<> struct ToUnsigned< int16_t> : HasType<uint16_t> {};
        template<> struct ToUnsigned< int32_t> : HasType<uint32_t> {};
        template<> struct ToUnsigned< int64_t> : HasType<uint64_t> {};
        template<> struct ToUnsigned<uint8_t > : HasType<uint8_t > {};
        template<> struct ToUnsigned<uint16_t> : HasType<uint16_t> {};
        template<> struct ToUnsigned<uint32_t> : HasType<uint32_t> {};
        template<> struct ToUnsigned<uint64_t> : HasType<uint64_t> {};

        template<> struct ToSigned<uint8_t > : HasType<int8_t > {};
        template<> struct ToSigned<uint16_t> : HasType<int16_t> {};
        template<> struct ToSigned<uint32_t> : HasType<int32_t> {};
        template<> struct ToSigned<uint64_t> : HasType<int64_t> {};
        template<> struct ToSigned< int8_t > : HasType<int8_t > {};
        template<> struct ToSigned< int16_t> : HasType<int16_t> {};
        template<> struct ToSigned< int32_t> : HasType<int32_t> {};
        template<> struct ToSigned< int64_t> : HasType<int64_t> {};
    }

    /**
     * An unsigned equivalent of type T
     */
    template <typename T>
    using ToUnsigned = typename Detail::ToUnsigned<T>::Type;

    /**
     * A signed equivalent of type T
     */
    template <typename T>
    using ToSigned = typename Detail::ToSigned<T>::Type;

    namespace NumeralTraits {
        /**
         * Tests whether or not the given type is an integer.
         */
        template<typename Type>
        inline constexpr bool IsInteger = NumeralTraits::Detail::IsInteger<Type>::Value;

        /**
         * Tests whether or not the given type is an floating point number.
         */
        template<typename Type>
        inline constexpr bool IsFloatingPoint = NumeralTraits::Detail::IsFloatingPoint<Type>::Value;

        /**
         * Tests whether or not the given type is an integer or a floating point number.
        */
        template<typename Type>
        inline constexpr bool IsNumber = NumeralTraits::Detail::IsNumber<Type>::Value;

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Woverflow"

        /**
         * Struct containing information about an integer type.
         */
        template<typename Type>
        struct Info {
            static_assert(IsInteger<Type>, "Type must be an integer");

            /**
             * Whether or not the type is a signed type.
             */
            constexpr const static bool IsSigned    = Type(-1) < Type(0);

            /**
             * Whether or not the type is an unsigned type.
             */
            constexpr const static bool IsUnsigned  = !IsSigned;

            /**
             * Minimum value that the type can store
             */
            constexpr const static Type MinimumValue = IsSigned ? static_cast<Type>(1U << (sizeof(Type) * 8 - 1)) : 0;

            /**
             * Maximum value that the type can store.
             */
            constexpr const static Type MaximumValue = MinimumValue - 1;
        };
        #pragma GCC diagnostic pop

    }  // namespace NumeralTraits
}  // namespace FunnyOS::Stdlib

#ifndef F_NO_GLOBAL_NUMERALS
    using    int8_t      = FunnyOS::Stdlib:: int8_t;
    using    int16_t     = FunnyOS::Stdlib:: int16_t;
    using    int32_t     = FunnyOS::Stdlib:: int32_t;
    using   uint8_t      = FunnyOS::Stdlib::uint8_t;
    using   uint16_t     = FunnyOS::Stdlib::uint16_t;
    using   uint32_t     = FunnyOS::Stdlib::uint32_t;
    using   int64_t      = FunnyOS::Stdlib:: int64_t;
    using   uint64_t     = FunnyOS::Stdlib::uint64_t;

    using  intmax_t    = FunnyOS::Stdlib:: intmax_t;
    using uintmax_t    = FunnyOS::Stdlib::uintmax_t;
    using  size_t      = FunnyOS::Stdlib:: size_t;
    using ssize_t      = FunnyOS::Stdlib::ssize_t;
    using  intptr_t    = FunnyOS::Stdlib:: intptr_t;
    using uintptr_t    = FunnyOS::Stdlib::uintptr_t;
    using ptrdiff_t    = FunnyOS::Stdlib::ptrdiff_t;
#endif
// clang-format on

#include "IntegerTypes.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
