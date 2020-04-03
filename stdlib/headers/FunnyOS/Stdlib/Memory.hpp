#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP

#include "IntegerTypes.hpp"

namespace FunnyOS::Stdlib::Memory {

    /**
     * A pointer to memory that also holds the size.
     *
     * @tparam T pointer type
     */
    template <typename T> struct SizedBuffer {
        /**
         * Pointer to memory
         */
        T* Data;

        /**
         * Size of the memory in bytes.
         */
        size_t Size;
    };

    /**
     * Copies [destination.Size] bytes from [source] to [destination.Data]
     */
    template <typename Type> inline void Copy(SizedBuffer<Type>& destination, const Type* source);

    /**
     * Copies [size] bytes from [source] to [destination]
     */
    template <typename Type> inline void Copy(Type* destination, const Type* source, size_t size);

    /**
     * Sets [destination.Size] at [destination.Data] to [byte]
     */
    template <typename Type> inline void Set(SizedBuffer<Type>& destination, Type byte);

    /**
     * Fills the whole [destination] with repeating patterns of [pattern]
     */
    template <typename Type> inline void Fill(SizedBuffer<Type>& destination, const SizedBuffer<Type>& pattern);

    /**
     * Allocates N bytes of memory on the heap.
     *
     * @param[in] size number of bytes to allocate may be zero.
     *
     * @return allocated memory or nullptr if there is not enough mermory
     */
    void* Allocate(size_t size);

    /**
     * Allocates N bytes of aligned memory on the heap.
     *
     * @param[in] size number of bytes to allocate may be zero.
     * @param[in] alignment memory alignment
     *
     * @return allocated memory or nullptr if there is not enough memory.
     *         Returned value is divisible by alignment if the allocator supports alignment.
     */
    void* AllocateAligned(size_t size, size_t alignment);

    /**
     * Allocates a SizedBuffer<T> of the heap.
     * If there is not enough memory the returned buffer's Size and Data will 0 and nullptr respectively.
     *
     * @tparam T type of the newly allocated buffer
     *
     * @param[in] size size of the memory to allocate
     *
     * @return newly allocated buffer.
     */
    template <typename T> inline SizedBuffer<T> AllocateBuffer(size_t size);

    /**
     * Allocates N bytes of memory on the heap and initialize it with zeros.
     *
     * @param[in] size number of bytes to allocate may be zero.
     *
     * @return zero-initialized allocated memory or nullptr if there is not enough memory.
     */
    void* AllocateInitialized(size_t size);

    /**
     * Reallocates the given memory previously allocated via any Allocate* or Reallocate* methods.
     * The contents of the newly allocated memory will be copied from the old memory.
     *
     * If the returned value != nullptr then the old memory will be freed as if Free(data) was called.
     *
     * @param[in, out] ptr memory to reallocate, is freed when returned value != nullptr
     * @param[in] size size of the memory block.
     * @return the newly allocated chunk or nullptr if not enough memory.
     */
    void* Reallocate(void* data, size_t size);

    /**
     * Frees a chunk of memory previously allocated via any Allocate* or Reallocate* methods.
     * Any other parameter will cause an undefined behaviour.
     *
     * @param[in, out] ptr memory to free
     */
    void Free(void* data);

}  // namespace FunnyOS::Stdlib::Memory

#include "Memory.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_MEMORY_HPP