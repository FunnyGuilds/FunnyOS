#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASH_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASH_HPP

#include "IntegerTypes.hpp"

namespace FunnyOS::Stdlib {

    using hash_t = size_t;

    template <typename T>
    struct Hash {
        hash_t operator()(const T& obj) {
            // TODO: Do it better
            return reinterpret_cast<hash_t>(&obj);
        }
    };

#define NUMERIC_HASH(type)                   \
    template <>                              \
    struct Hash<type> {                      \
        hash_t operator()(const type& obj) { \
            return static_cast<hash_t>(obj); \
        }                                    \
    };

    NUMERIC_HASH(int8_t);
    NUMERIC_HASH(int16_t);
    NUMERIC_HASH(int32_t);
    NUMERIC_HASH(int64_t);
    NUMERIC_HASH(uint8_t);
    NUMERIC_HASH(uint16_t);
    NUMERIC_HASH(uint32_t);
    NUMERIC_HASH(uint64_t);

#undef NUMERIC_HASH

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASH_HPP
