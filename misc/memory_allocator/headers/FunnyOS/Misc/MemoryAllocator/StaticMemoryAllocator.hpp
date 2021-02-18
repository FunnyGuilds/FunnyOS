#ifndef FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICMEMORYALLOCATOR_HPP
#define FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICMEMORYALLOCATOR_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

#include "MemoryAllocator.hpp"

namespace FunnyOS::Misc::MemoryAllocator {

    /**
     * Type of a memory.
     * The values in this enum should be random constants because if the block would be accidentally overridden those
     * values will be also overwritten thus indicating an issue with the block.
     */
    enum class MemoryMetaStatus : memoryaddress_t { Taken = 0xA594EDF9, Freed = 0xBB4272BE, Invalid = 0 };

    /**
     * Memory meta block. This block is guaranteed to be present just before an allocated memory region.
     */
    struct MemoryMetaBlock {
        /**
         * Status of the memory block.
         */
        MemoryMetaStatus Status;

        /**
         * Size of the block in bytes.
         */
        memoryaddress_t BlockSize;

        /**
         * If Status == Freed, then this points to the next free block in the memory or is 0 if this is the last free
         * block.
         * If Status == Taken, this is always 0
         */
        memoryaddress_t NextFreeBlock;

        /**
         * Gets the next free block (from memory address NextFreeBlock), or nullptr if NextFreeBlock is 0
         */
        [[nodiscard]] MemoryMetaBlock* GetNext() const noexcept;
    };

    /**
     * Simple and efficient memory allocator, tracking all free blocks and merging them if possible.
     */
    class StaticMemoryAllocator : public IMemoryAllocator {
       public:
        /**
         * Initializes the memory allocator.
         *
         * @param memoryStart[in] the first address, where the allocator will start allocations
         * @param memoryEnd[in] last address that the allocator can address + 1
         */
        void Initialize(memoryaddress_t memoryStart, memoryaddress_t memoryEnd) noexcept;

        void* Allocate(size_t size, size_t alignment) noexcept override;

        void Free(void* ptr) noexcept override;

        void* Reallocate(void* ptr, size_t size, size_t alignment) noexcept override;

        size_t GetTotalFreeMemory() const noexcept override;

        size_t GetAllocatedMemory() const noexcept override;

        size_t GetTotalAvailableMemory() const noexcept override;

        /**
         * Gets a size of a memory block that was allocated using Allocate before.
         *
         * @param ptr ptr returned by Allocate
         */
        size_t GetMemoryBlockSize(void *ptr);

        /**
         * Gets the highest memory address that this allocator ever allocated.
         * That is the (highest address + 1) of the top-most block.
         *
         * @return highest memory address used by the allocator.
         */
        [[nodiscard]] memoryaddress_t GetCurrentMemoryTop() const noexcept;

        /**
         * Returns the lowest address this allocator will use when allocating new data (the bottom memory boundary).
         * This value is inclusive.
         *
         * @return lowest address this allocator will use, inclusive
         */
        [[nodiscard]] memoryaddress_t GetMemoryStart() const noexcept;

        /**
         * Returns the highest address this allocator will use when allocating new data (the top memory boundary).
         * This value is exclusive.
         *
         *
         * @return highest address this allocator will use, exclusive
         */
        [[nodiscard]] memoryaddress_t GetMemoryEnd() const noexcept;

       private:
        /**
         * Finds a block of a size equal or greater than size and returns a block that precedes it in the free memory
         * list.
         *
         * @param[in] size minimum size of the block
         * @return suitable block predecessor or nullptr
         */
        [[nodiscard]] MemoryMetaBlock* FindFreeBlockPredecessor(size_t size, size_t alignment) noexcept;

        /**
         * Splits the successor of the given free block in two blocks.
         * If the block is possible to split then the resulting blocks are:
         * - the first one has a size of minimum [size], is marked as Taken and removed from free memory list.
         * - the second one has a size of [original block size - size - sizeof(MemoryMetaBlock)] and is put to the free
         * memory list.
         *
         * If the block is impossible to split the successor is marked as Taken, remove from free memory list and
         * returned.
         *
         * @param[in] predecessor predecessor of the block to be split.
         * @param[in] size minimum size of the returned block
         * @return a memory block marked as Taken of size greater or equal [size]. Never nullptr.
         */
        [[nodiscard]] MemoryMetaBlock* SplitBlockAndTakeItIfPossible(
            MemoryMetaBlock* predecessor, size_t size) noexcept;

        /**
         * Allocates a new block after the end of the last allocated block.
         *
         * @param[in] size size of the block to be allocated
         * @return the allocated block of size [size], marked as Taken
         */
        [[nodiscard]] MemoryMetaBlock* AllocateNewBlock(size_t size, size_t alignment) noexcept;

        /**
         * Merges clumps of Freed marked zones in the free memory list to bigger clumps.
         *
         * @return whether or not at least one merge occurred.
         */
        bool MergeMemory() noexcept;

       private:
        MemoryMetaBlock* m_firstFreeBlock;
        MemoryMetaBlock* m_lastFreeBlock;
        memoryaddress_t m_currentMemory;
        memoryaddress_t m_memoryStart;
        memoryaddress_t m_memoryEnd;
        size_t m_totalMemory;
        size_t m_usedMemory;
    };

}  // namespace FunnyOS::Misc::MemoryAllocator

#endif  // FUNNYOS_MISC_MEMORY_ALLOCATOR_HEADERS_FUNNYOS_MISC_MEMORYALLOCATOR_STATICMEMORYALLOCATOR_HPP
