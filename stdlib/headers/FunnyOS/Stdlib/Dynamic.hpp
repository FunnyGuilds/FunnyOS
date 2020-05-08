#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP

#include "Memory.hpp"

namespace FunnyOS::Stdlib {

    /**
     * A SizedBuffer wrapper that will automatically allocate data on construction and free on destruction.
     *
     * All Memory::*Buffer functions are still valid on a SmartSizedBuffer.
     *
     * @tparam T pointer type
     */
    template <typename T>
    struct SmartSizedBuffer : public Memory::SizedBuffer<T> {
       public:
        COPYABLE(SmartSizedBuffer);
        MOVEABLE(SmartSizedBuffer);

        /**
         * Consrtucts the buffer and allocates [size] * sizeof(T) bytes of data.
         *
         * @param size size of the buffer
         */
        SmartSizedBuffer(size_t size);

        /**
         * Destructs the buffer and frees its data.
         */
        virtual ~SmartSizedBuffer();
    };

}  // namespace FunnyOS::Stdlib

#include "Dynamic.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_DYNAMIC_HPP
