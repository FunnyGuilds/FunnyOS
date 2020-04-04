#ifndef FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#define FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
#include "TypeTraits.hpp"
#include "IntegerTypes.tcc"

namespace FunnyOS::Stdlib::NumeralTypes {
    // clang-format off
    using  int8_t       =           char;            // NOLINT(google-runtime-int)
    using  int16_t      =           short;           // NOLINT(google-runtime-int)
    using  int32_t      =           int;             // NOLINT(google-runtime-int)
    using  int64_t      =           long long int;   // NOLINT(google-runtime-int)
    using uint8_t       =  unsigned char;            // NOLINT(google-runtime-int)
    using uint16_t      =  unsigned short;           // NOLINT(google-runtime-int)
    using uint32_t      =  unsigned int;             // NOLINT(google-runtime-int)
    using uint64_t      =  unsigned long long int;   // NOLINT(google-runtime-int)

    #ifdef FOS_64
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

    namespace Traits {
        /**
         * Tests whether or not the given type is an integer.
         */
        template<typename Type>
        inline constexpr bool IsInteger = Traits::Detail::IsInteger<Type>::Value;

        /**
         * Tests whether or not the given type is an floating point number.
         */
        template<typename Type>
        inline constexpr bool IsFloatingPoint = Traits::Detail::IsFloatingPoint<Type>::Value;

        /**
         * Tests whether or not the given type is an integer or a floating point number.
        */
        template<typename Type>
        inline constexpr bool IsNumber = Traits::Detail::IsNumber<Type>::Value;

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

    }  // namespace Traits
}  // namespace FunnyOS::Stdlib::NumeralTypes

#ifndef FUNNYOS_NO_GLOBAL_NUMERALS
    using    int8_t      = FunnyOS::Stdlib::NumeralTypes::int8_t;
    using    int16_t     = FunnyOS::Stdlib::NumeralTypes::int16_t;
    using    int32_t     = FunnyOS::Stdlib::NumeralTypes::int32_t;
    using   uint8_t      = FunnyOS::Stdlib::NumeralTypes::uint8_t;
    using   uint16_t     = FunnyOS::Stdlib::NumeralTypes::uint16_t;
    using   uint32_t     = FunnyOS::Stdlib::NumeralTypes::uint32_t;
    using   int64_t      = FunnyOS::Stdlib::NumeralTypes::int64_t;
    using   uint64_t     = FunnyOS::Stdlib::NumeralTypes::uint64_t;


    using  intmax_t    = FunnyOS::Stdlib::NumeralTypes:: intmax_t;
    using uintmax_t    = FunnyOS::Stdlib::NumeralTypes::uintmax_t;
    using  size_t      = FunnyOS::Stdlib::NumeralTypes:: size_t;
    using ssize_t      = FunnyOS::Stdlib::NumeralTypes::ssize_t;
    using  intptr_t    = FunnyOS::Stdlib::NumeralTypes:: intptr_t;
    using uintptr_t    = FunnyOS::Stdlib::NumeralTypes::uintptr_t;
    using ptrdiff_t    = FunnyOS::Stdlib::NumeralTypes::ptrdiff_t;
#endif
// clang-format on

#include "IntegerTypes.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_INTEGERTYPES_HPP
