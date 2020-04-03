#ifndef FUNNYOS_BOOTLOADER_COMMONS_SRC_LOWMEMORYALLOCATOR_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_SRC_LOWMEMORYALLOCATOR_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootloader {

    /**
     * We don't really need larger memory addresses since we are operating in the 0x00007C00 - 0x0007FFFF memory section
     */
    using memoryaddress_t = uint32_t;

    /**
     * Type of a memory.
     * The values in this enum should be random constants because if the block would be accidentally overridden those
     * values will be also overwritten thus indicating an issue with the block.
     */
    enum class MemoryMetaStatus : memoryaddress_t { Taken = 0x94EF, Freed = 0x72BE, Invalid = 0 };

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
        [[nodiscard]] MemoryMetaBlock* GetNext() noexcept;
    };

    /**
     * Simple and efficient memory allocator, tracking all free blocks and merging them if possible.
     */
    class LowMemoryAllocator {
       public:
        /**
         * Initializes the memory allocator.
         *
         * @param memoryStart[in] the first address, where the allocator will start allocations
         * @param memoryEnd[in] last address that the allocator can address + 1
         */
        void Initialize(memoryaddress_t memoryStart, memoryaddress_t memoryEnd) noexcept;

        /**
         * Allocates a chunk of memory.
         *
         * @param[in] size size of the memory
         * @return the newly allocated chunk or nullptr if not enough memory.
         */
        [[nodiscard]] void* Allocate(size_t size) noexcept;

        /**
         * Frees a chunk of memory previously allocated via Allocate or Reallocate.
         * Any other parameter will cause an undefined behaviour.
         *
         * @param[in,out] ptr memory to free
         */
        void Free(void* ptr) noexcept;

        /**
         * Reallocates the given memory previously allocated via Allocate or Reallocate.
         * The contents of the newly allocated memory will be copied from the old memory.
         * The old memory will be freed.
         *
         * @param[in,out] ptr memory, is is freed when return value of this function is not nullptr
         * @param[in] size size of the memory block.
         * @return the newly allocated chunk or nullptr if not enough memory.
         */
        [[nodiscard]] void* Reallocate(void* ptr, size_t size) noexcept;

       private:
        /**
         * Finds a block of a size equal or greater than size and returns a block that precedes it in the free memory
         * list.
         *
         * @param[in] size minimum size of the block
         * @return suitable block predecessor or nullptr
         */
        [[nodiscard]] MemoryMetaBlock* FindFreeBlockPredecessor(size_t size) noexcept;

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
        [[nodiscard]] MemoryMetaBlock* SplitBlockAndTakeItIfPossible(MemoryMetaBlock* predecessor, size_t size) noexcept;

        /**
         * Allocates a new block after the end of the last allocated block.
         *
         * @param[in] size size of the block to be allocated
         * @return the allocated block of size [size], marked as Taken
         */
        [[nodiscard]] MemoryMetaBlock* AllocateNewBlock(size_t size) noexcept;

        /**
         * Merges clumps of Freed marked zones in the free memory list to bigger clumps.
         *
         * @return whether or not at least one merge occurred.
         */
        bool MergeMemory() noexcept;

       private:
        MemoryMetaBlock* m_firstFreeBlock;
        memoryaddress_t m_currentMemory;
        memoryaddress_t m_memoryEnd;
    };

}  // namespace FunnyOS::Bootloader

#endif  // FUNNYOS_BOOTLOADER_COMMONS_SRC_LOWMEMORYALLOCATOR_HPP
