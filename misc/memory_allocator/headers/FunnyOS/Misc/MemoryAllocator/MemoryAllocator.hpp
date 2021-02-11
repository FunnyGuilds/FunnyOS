#ifndef FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_MEMORYALLOCATOR_HPP
#define FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_MEMORYALLOCATOR_HPP

#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Misc::MemoryAllocator {

    /**
     * Memory address integer representation.
     */
    using memoryaddress_t = uintmax_t;

    class IMemoryAllocator {
        INTERFACE(IMemoryAllocator);

       public:
        /**
         * Allocates a chunk of memory.
         *
         * @param[in] size size of the memory
         * @param[in] alignment memory alignment
         * @return the newly allocated chunk or nullptr if not enough memory.
         */
        [[nodiscard]] virtual void* Allocate(size_t size, size_t alignment) noexcept = 0;

        /**
         * Frees a chunk of memory previously allocated via Allocate or Reallocate.
         * Any other parameter will cause an undefined behaviour.
         *
         * @param[in,out] ptr memory to free
         */
        virtual void Free(void* ptr) noexcept = 0;

        /**
         * Reallocates the given memory previously allocated via Allocate or Reallocate.
         * The contents of the newly allocated memory will be copied from the old memory.
         * The old memory will be freed.
         *
         * @param[in,out] ptr memory, is is freed when return value of this function is not nullptr
         * @param[in] size size of the memory block.
         * @param[in] alignment memory alignment
         * @return the newly allocated chunk or nullptr if not enough memory.
         */
        [[nodiscard]] virtual void* Reallocate(void* ptr, size_t size, size_t alignment) noexcept = 0;
    };

}  // namespace FunnyOS::Misc::MemoryAllocator

#endif  // FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_MEMORYALLOCATOR_HPP
