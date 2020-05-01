#include "Paging.hpp"

#include <FunnyOS/Stdlib/Memory.hpp>
#include "Logging.hpp"

#include <BootloaderConfig.hpp>

namespace FunnyOS::Bootloader32 {
    namespace {
        constexpr size_t PAGE_SIZE = 0x1000;
        constexpr size_t PAGE_ENTRY_SIZE = 8;  // 8 bytes in long mode
        constexpr size_t ENTRIES_PER_PAGE = (PAGE_SIZE / PAGE_ENTRY_SIZE);
        constexpr uint64_t SINGLE_TABLE_MAPPING_SIZE = PAGE_SIZE * ENTRIES_PER_PAGE;
    }  // namespace

    uintmax_t AlignToPage(uintmax_t memory) {
        if (memory % PAGE_SIZE == 0) {
            return memory;
        }

        return memory + (PAGE_SIZE - (memory % PAGE_SIZE));
    }

    uint64_t GetKernelVirtualLocation() {
        return F_KERNEL_VIRTUAL_ADDRESS;
    }

    void* AllocateClearPage(StaticMemoryAllocator& allocator) {
        void* page = allocator.Allocate(PAGE_SIZE, PAGE_SIZE);
        Stdlib::Memory::SizedBuffer<uint8_t> buffer{reinterpret_cast<uint8_t*>(page), PAGE_SIZE};
        Stdlib::Memory::Set<uint8_t>(buffer, 0);
        return page;
    }

    void* GetPageTable(void* currentBase, uint64_t virtualAddress, unsigned int level,
                       StaticMemoryAllocator& allocator) {
        F_ASSERT(((reinterpret_cast<uintptr_t>(currentBase) % PAGE_SIZE) == 0), "virtual address not page aligned");
        if (level == 1) {
            return currentBase;
        }

        unsigned int currentIndex = (reinterpret_cast<uint64_t>(virtualAddress) >> (12 + (level - 1) * 9)) & 0x1FF;
        auto* entries = static_cast<uint64_t*>(currentBase);

        if ((entries[currentIndex] & 1) == 0) {
            // Flags 0b11 - supervisor only, read/write, present
            void* entry = AllocateClearPage(allocator);
            entries[currentIndex] = reinterpret_cast<uint64_t>(entry) | 0b11;
        }

        void* nextBase = reinterpret_cast<void*>(entries[currentIndex] & 0xFFFFFFF000);

        return GetPageTable(nextBase, virtualAddress, level - 1, allocator);
    }

    void MapSingleTable(void* pml4base, uint64_t virtualAddress, uint64_t physicalAddress,
                        StaticMemoryAllocator& allocator) {
        F_ASSERT((virtualAddress % SINGLE_TABLE_MAPPING_SIZE) == 0, "Virtual address not aligned");
        F_ASSERT((physicalAddress % PAGE_SIZE) == 0, "Physical address not aligned");

        FB_LOG_DEBUG_F("Maping table %016llx to %016llx", virtualAddress, physicalAddress);

        void* table = GetPageTable(pml4base, virtualAddress, 4, allocator);
        auto* tableEntries = static_cast<uint64_t*>(table);

        for (size_t i = 0; i < ENTRIES_PER_PAGE; i++) {
            // Flags 0b11 - supervisor only, read/write, present
            tableEntries[i] = ((reinterpret_cast<uint64_t>(physicalAddress) + (i * PAGE_SIZE))) | 0b11;
        }
    }

    void* SetupInitialKernelPages(uintmax_t location, uintmax_t kernelSize, StaticMemoryAllocator& pageTableAllocator) {
        void* pml4base = AllocateClearPage(pageTableAllocator);

        // Identity map first 1MB
        MapSingleTable(pml4base, 0, 0, pageTableAllocator);

        // Map kernel memory
        for (uint64_t i = 0; i < (kernelSize / SINGLE_TABLE_MAPPING_SIZE) + 1; i++) {
            uint64_t virtualAddr = GetKernelVirtualLocation() + i * SINGLE_TABLE_MAPPING_SIZE;
            uint64_t physicalAddr = static_cast<uint64_t>(location) + i * SINGLE_TABLE_MAPPING_SIZE;

            MapSingleTable(pml4base, virtualAddr, physicalAddr, pageTableAllocator);
        }

        return pml4base;
    }
}  // namespace FunnyOS::Bootloader32
