#ifndef FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICFRAGMENTEDMEMORYALLOCATOR_HPP
#define FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICFRAGMENTEDMEMORYALLOCATOR_HPP

#include <FunnyOS/Stdlib/Dynamic.hpp>
#include <FunnyOS/Stdlib/Vector.hpp>

#include "StaticMemoryAllocator.hpp"

namespace FunnyOS::Misc::MemoryAllocator {

    /**
     * Represents a region in memory that can be used for allocation
     */
    struct MemoryFragment {
        /**
         * Start of the region (inclusive)
         */
        memoryaddress_t RegionStart;

        /**
         * End of the region (exclusive)
         */
        memoryaddress_t RegionEnd;
    };

    /**
     * An IMemoryAllocator implementation that uses multiple StaticMemoryAllocators to allocate in fragmented memory.
     */
    class StaticFragmentedMemoryAllocator : public IMemoryAllocator {
       public:
        /**
         * Initializes the allocator.
         *
         * @param memoryFragments fragments of memory to be used for allocation, the fragments cannot intersect
         */
        void Initialize(const Stdlib::Memory::SizedBuffer<MemoryFragment>& memoryFragments);

        void* Allocate(size_t size, size_t alignment) noexcept override;

        void Free(void* ptr) noexcept override;

        void* Reallocate(void* ptr, size_t size, size_t alignment) noexcept override;

        size_t GetTotalFreeMemory() const noexcept override;

        size_t GetAllocatedMemory() const noexcept override;

        size_t GetTotalAvailableMemory() const noexcept override;

        const Stdlib::Vector<StaticMemoryAllocator>& GetMemberAllocators() const;

        Stdlib::Vector<StaticMemoryAllocator>& GetMemberAllocators();

       private:
        void* DoReallocate(StaticMemoryAllocator& alloc, void* ptr, size_t size, size_t alignment);

       private:
        Stdlib::Vector<StaticMemoryAllocator> m_allocators;
    };

}  // namespace FunnyOS::Misc::MemoryAllocator

#endif  // FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICFRAGMENTEDMEMORYALLOCATOR_HPP
