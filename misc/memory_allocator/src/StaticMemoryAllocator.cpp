#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Misc::MemoryAllocator {
    using namespace FunnyOS::Stdlib;

    MemoryMetaBlock* MemoryMetaBlock::GetNext() const noexcept {
        return reinterpret_cast<MemoryMetaBlock*>(NextFreeBlock);
    }

    namespace {
        /**
         * Converts a pointer to  memoryaddress_t
         */
        template <typename T>
        inline memoryaddress_t PtrToAddress(T* ptr) {
            return reinterpret_cast<uintptr_t>(ptr);
        }

        /**
         * Converts a memoryaddress_t to pointer of the given type.
         */
        template <typename T>
        inline T* AddressToPtr(memoryaddress_t address) {
            return reinterpret_cast<T*>(address);
        }

        /**
         * Gets the address at which memory described by the given block starts.
         */
        inline void* GetBlockMemory(MemoryMetaBlock* block) {
            auto* blockAddress = reinterpret_cast<uint8_t*>(block);
            return blockAddress + sizeof(MemoryMetaBlock);
        }

        /**
         * Gets a MemoryMetaBlock that describes memory that starts at ptr.
         */
        inline MemoryMetaBlock* GetMemoryBlock(void* ptr) {
            auto* blockAddress = static_cast<uint8_t*>(ptr);
            return reinterpret_cast<MemoryMetaBlock*>(blockAddress - sizeof(MemoryMetaBlock));
        }

        bool IsAlignedTo(void* memory, size_t alignment) {
            return (reinterpret_cast<memoryaddress_t>(memory) % alignment) == 0;
        }

        bool IsAlignedTo(memoryaddress_t address, size_t alignment) {
            return (address % alignment) == 0;
        }

        memoryaddress_t AlignAddress(memoryaddress_t address, size_t alignment) {
            if (IsAlignedTo(address, alignment)) {
                return address;
            }

            return address + (alignment - (address % alignment));
        }

        memoryaddress_t AlignAddress(void* memory, size_t alignment) {
            return AlignAddress(reinterpret_cast<memoryaddress_t>(memory), alignment);
        }

    }  // namespace

    void StaticMemoryAllocator::Initialize(memoryaddress_t memoryStart, memoryaddress_t memoryEnd) noexcept {
        m_currentMemory = memoryStart;
        m_memoryEnd     = memoryEnd;

        m_firstFreeBlock = AddressToPtr<MemoryMetaBlock>(m_currentMemory);
        m_lastFreeBlock  = m_firstFreeBlock;

        m_firstFreeBlock->Status        = MemoryMetaStatus::Freed;
        m_firstFreeBlock->BlockSize     = 0;
        m_firstFreeBlock->NextFreeBlock = 0;

        m_currentMemory += sizeof(MemoryMetaBlock);
    }

    void* StaticMemoryAllocator::Allocate(size_t size, size_t alignment) noexcept {
        // Find a suitable free block
        auto* freeBlockPredecessor = FindFreeBlockPredecessor(size, alignment);

        // If found split it if possible, mark as taken and return.
        if (freeBlockPredecessor != nullptr) {
            MemoryMetaBlock* currentBlock = SplitBlockAndTakeItIfPossible(freeBlockPredecessor, size);
            return GetBlockMemory(currentBlock);
        }

        // If no free blocks with suitable size are found, allocate new one.
        MemoryMetaBlock* newBlock = AllocateNewBlock(size, alignment);
        if (newBlock == nullptr) {
            return nullptr;
        }

        return GetBlockMemory(newBlock);
    }

    void StaticMemoryAllocator::Free(void* ptr) noexcept {
        auto* block = GetMemoryBlock(ptr);

        if (block->Status == MemoryMetaStatus::Freed) {
            // whatever, maybe this should be reported?
            return;
        }

        F_ASSERT(block->Status == MemoryMetaStatus::Taken, "Invalid block status");

        // Insert this block to free memory list, making sure that the memory stays sorted after the insert.
        MemoryMetaBlock* blockToInsertAfter = m_firstFreeBlock;
        while (blockToInsertAfter->NextFreeBlock != 0 && blockToInsertAfter->NextFreeBlock < PtrToAddress(block)) {
            blockToInsertAfter = blockToInsertAfter->GetNext();
        }

        block->NextFreeBlock              = blockToInsertAfter->NextFreeBlock;
        blockToInsertAfter->NextFreeBlock = PtrToAddress(block);

        if (block->NextFreeBlock == 0) {
            m_lastFreeBlock = block;
        }

        // Mark it as free
        block->Status = MemoryMetaStatus::Freed;

        // If the free has created any Freed memory clumps we should try to merge them.
        MergeMemory();
    }

    void* StaticMemoryAllocator::Reallocate(void* ptr, size_t size, size_t alignment) noexcept {
        auto* oldMemoryBlock = GetMemoryBlock(ptr);
        F_ASSERT(oldMemoryBlock->Status == MemoryMetaStatus::Taken, "attempting to reallocate invalid block");

        // If the new block size is equal to the old block size there is no point in reallocating
        if (size == oldMemoryBlock->BlockSize) {
            return ptr;
        }

        // Allocate new block
        void* newMemory = Allocate(size, alignment);
        if (newMemory == nullptr) {
            return nullptr;
        }

        // Copy data
        Memory::Copy(newMemory, ptr, Min(static_cast<size_t>(oldMemoryBlock->BlockSize), size));

        // Free old memory
        Free(ptr);
        return newMemory;
    }

    memoryaddress_t StaticMemoryAllocator::GetCurrentMemoryTop() const noexcept {
        return m_currentMemory;
    }

    memoryaddress_t StaticMemoryAllocator::GetMemoryEnd() const noexcept {
        return m_memoryEnd;
    }

    MemoryMetaBlock* StaticMemoryAllocator::FindFreeBlockPredecessor(size_t size, size_t alignment) noexcept {
        MemoryMetaBlock* previousBlock;
        MemoryMetaBlock* currentBlock = m_firstFreeBlock;

        while (currentBlock->NextFreeBlock != 0) {
            previousBlock = currentBlock;
            currentBlock  = AddressToPtr<MemoryMetaBlock>(currentBlock->NextFreeBlock);

            // Sanity check
            F_ASSERT(currentBlock->Status == MemoryMetaStatus::Freed, "Non-freed block found in free block list");

            if (currentBlock->BlockSize < size) {
                continue;
            }

            if (!IsAlignedTo(GetBlockMemory(currentBlock), alignment)) {
                continue;
            }

            // Found suitable block
            return previousBlock;
        }

        // No blocks found
        return nullptr;
    }

    MemoryMetaBlock* StaticMemoryAllocator::SplitBlockAndTakeItIfPossible(
        MemoryMetaBlock* predecessor, size_t size) noexcept {
        F_ASSERT(predecessor->NextFreeBlock != 0, "predecessor has no actual next value");

        auto* currentBlock = predecessor->GetNext();
        if (currentBlock->BlockSize <= size + sizeof(MemoryMetaBlock)) {
            // Block is too small, can't split, just mark it as taken and remove from the list.
            predecessor->NextFreeBlock  = currentBlock->NextFreeBlock;
            currentBlock->Status        = MemoryMetaStatus::Taken;
            currentBlock->NextFreeBlock = 0;
            return currentBlock;
        }

        // Create new MemoryMetaBlock struct after the first block.
        auto* newFreeBlock =
            reinterpret_cast<MemoryMetaBlock*>(reinterpret_cast<uint8_t*>(GetBlockMemory(currentBlock)) + size);

        // Setup MemoryMetaBlock for the second block.
        newFreeBlock->Status        = MemoryMetaStatus::Freed;
        newFreeBlock->BlockSize     = currentBlock->BlockSize - size - sizeof(MemoryMetaBlock);
        newFreeBlock->NextFreeBlock = currentBlock->NextFreeBlock;

        // Setup MemoryMetaBlock for the first block
        currentBlock->Status        = MemoryMetaStatus::Taken;
        currentBlock->BlockSize     = size;
        currentBlock->NextFreeBlock = 0;

        // Update list
        predecessor->NextFreeBlock = PtrToAddress(newFreeBlock);

        return currentBlock;
    }

    MemoryMetaBlock* StaticMemoryAllocator::AllocateNewBlock(size_t size, size_t alignment) noexcept {
        F_ASSERT(alignment > 0, "alignment == 0");

        // Create new memory block at the end of current memory
        auto* newMetaBlock = AddressToPtr<MemoryMetaBlock>(m_currentMemory);

        if (!IsAlignedTo(GetBlockMemory(newMetaBlock), alignment)) {
            memoryaddress_t newAlignedAddress = AlignAddress(GetBlockMemory(newMetaBlock), alignment);

            // As long as the aligning block is smaller than its meta block there is no point in creating in there
            while (newAlignedAddress - PtrToAddress(GetBlockMemory(newMetaBlock)) <= sizeof(MemoryMetaBlock)) {
                // Just skip to the next aligned address
                newAlignedAddress += alignment;
            }

            // Create alignment block
            newMetaBlock->Status        = MemoryMetaStatus::Freed;
            newMetaBlock->BlockSize     = newAlignedAddress - m_currentMemory - sizeof(MemoryMetaBlock) * 2;
            newMetaBlock->NextFreeBlock = 0;

            m_lastFreeBlock->NextFreeBlock = PtrToAddress(newMetaBlock);
            m_lastFreeBlock                = newMetaBlock;
            m_currentMemory += newMetaBlock->BlockSize + sizeof(MemoryMetaBlock);

            // Update meta block
            newMetaBlock = AddressToPtr<MemoryMetaBlock>(m_currentMemory);
        }

        const memoryaddress_t lastByte = m_currentMemory + sizeof(MemoryMetaBlock) + size;
        F_ASSERT(IsAlignedTo(GetBlockMemory(newMetaBlock), alignment), "alignment failed");

        if (lastByte > m_memoryEnd) {
            // Oops, we reached end of our memory space.
            return nullptr;
        }

        // Increment current memory to be pointing just after the newMetaBlock
        m_currentMemory = lastByte;

        // Setup the block and return.
        newMetaBlock->Status        = MemoryMetaStatus::Taken;
        newMetaBlock->BlockSize     = size;
        newMetaBlock->NextFreeBlock = 0;
        return newMetaBlock;
    }

    bool StaticMemoryAllocator::MergeMemory() noexcept {
        MemoryMetaBlock* currentBlock = m_firstFreeBlock;

        // Never merge the first block, it is only a marker.
        if (currentBlock->NextFreeBlock == 0) {
            return false;
        }
        currentBlock = currentBlock->GetNext();

        bool didMerge = false;

        // Iterate over all entries until the end of the list
        while (currentBlock->NextFreeBlock != 0) {
            const memoryaddress_t addressAfter = PtrToAddress(GetBlockMemory(currentBlock)) + currentBlock->BlockSize;

            // Sanity check, no valid function should ever do any operation on free memory list that would make it
            // the list unsorted.
            F_ASSERT(addressAfter > PtrToAddress(currentBlock), "free blocks not sorted");

            // If the memory block ends exactly where the next free memory block is ...
            if (currentBlock->NextFreeBlock != addressAfter) {
                currentBlock = currentBlock->GetNext();
                continue;
            }

            // ... we can merge them.
            auto* nextBlock = currentBlock->GetNext();
            F_ASSERT(currentBlock->Status == MemoryMetaStatus::Freed, "Non-freed block find in free block list");
            F_ASSERT(nextBlock->Status == MemoryMetaStatus::Freed, "Non-freed block find in free block list");

            // Update size of the now-merged memory block.
            currentBlock->BlockSize += nextBlock->BlockSize + sizeof(MemoryMetaBlock);

            // Remove the second block from the list as it is no longer valid
            currentBlock->NextFreeBlock = nextBlock->NextFreeBlock;

            // Don't update currentBlock, process it one more time as more merges may possible for it
            didMerge = true;
        }

        return didMerge;
    }

}  // namespace FunnyOS::Misc::MemoryAllocator