#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>

namespace FunnyOS::Bootloader32 {
    using Misc::MemoryAllocator::StaticMemoryAllocator;

    /**
     * Size of a page in bytes.
     */
    constexpr size_t PAGE_SIZE = 0x1000;

    /**
     * Size, in bytes, of a single entry in a page map structure.
     */
    constexpr size_t PAGE_ENTRY_SIZE = 8;

    /**
     * How many entries can be put in one page map structure.
     */
    constexpr size_t ENTRIES_PER_PAGE = (PAGE_SIZE / PAGE_ENTRY_SIZE);

    /**
     * Size of a single simple mapping that SimplePageTableAllocator is capable of creating.
     */
    constexpr uint64_t SINGLE_TABLE_MAPPING_SIZE = PAGE_SIZE * ENTRIES_PER_PAGE;

    /**
     * Aligns the memory address to SINGLE_TABLE_MAPPING_SIZE upwards (the aligned address will be equal or smaller than
     * [memory]).
     *
     * @param memory address to align
     * @return aligned address
     */
    uintmax_t AlignToMappingUpwards(uintmax_t memory);
    /**
     * Aligns the memory address to PAGE_SIZE downwards (the aligned address will be equal or bigger than [memory]).
     * @param memory address to align
     * @return aligned address
     */
    uintmax_t AlignToPage(uintmax_t memory);

    /**
     * @return the virtual location at where the kernel should be loaded at.
     */
    uint64_t GetKernelVirtualLocation();

    /**
     * Simple allocator for page tables.
     */
    class SimplePageTableAllocator {
       public:
        /**
         * Creates new SimplePageTableAllocator
         *
         * @param allocator allocator to be used to allocate page map structures.
         */
        SimplePageTableAllocator(StaticMemoryAllocator& allocator);

        /**
         * Maps a memory region in the page table structure.
         *
         * @param location location at where to start the mapping, must be aligned to [PAGE_SIZE.
         * @param size size of the memory to map. Will be rounded up to a multiple of SINGLE_TABLE_MAPPING_SIZE.
         * @param virtualLocationBase virtual address for the mapping must be a aligned to [SINGLE_TABLE_MAPPING_SIZE]
         */
        void MapLocation(uint64_t location, uint64_t size, uint64_t virtualLocationBase);

        /**
         * @return the PML4 base address.
         */
        [[nodiscard]] void* GetPml4Base() const;

       private:
        void* AllocateClearPage();

        void* GetPageTable(void* currentBase, uint64_t virtualAddress, unsigned int level);

        void MapSingleTable(uint64_t virtualAddress, uint64_t physicalAddress);

       private:
        StaticMemoryAllocator& m_pageTableAllocator;
        void* m_pml4base;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP
