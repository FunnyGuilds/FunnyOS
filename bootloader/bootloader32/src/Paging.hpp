#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>

namespace FunnyOS::Bootloader32 {
    using Misc::MemoryAllocator::StaticMemoryAllocator;

    constexpr size_t PAGE_SIZE = 0x1000;
    constexpr size_t PAGE_ENTRY_SIZE = 8;
    constexpr size_t ENTRIES_PER_PAGE = (PAGE_SIZE / PAGE_ENTRY_SIZE);
    constexpr uint64_t SINGLE_TABLE_MAPPING_SIZE = PAGE_SIZE * ENTRIES_PER_PAGE;

    uintmax_t AlignToMappingUpwards(uintmax_t memory);

    uintmax_t AlignToPage(uintmax_t memory);

    uint64_t GetKernelVirtualLocation();

    class SimplePageTableAllocator {
       public:
        SimplePageTableAllocator(StaticMemoryAllocator& allocator);

        void MapLocation(uint64_t location, uint64_t size, uint64_t virtualLocationBase);

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
