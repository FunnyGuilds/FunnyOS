#include "Paging.hpp"

#include <FunnyOS/Stdlib/Memory.hpp>
#include "Logging.hpp"

#include <BootloaderConfig.hpp>

namespace FunnyOS::Bootloader32 {
    uintmax_t AlignToMappingUpwards(uintmax_t memory) {
        if (memory % SINGLE_TABLE_MAPPING_SIZE == 0) {
            return memory;
        }

        return memory - (memory % SINGLE_TABLE_MAPPING_SIZE);
    }

    uintmax_t AlignToPage(uintmax_t memory) {
        if (memory % PAGE_SIZE == 0) {
            return memory;
        }

        return memory + (PAGE_SIZE - (memory % PAGE_SIZE));
    }

    uint64_t GetKernelVirtualLocation() {
        return F_KERNEL_VIRTUAL_ADDRESS;
    }

    uint64_t GetPhysicalMemoryVirtualLocation() {
        return F_KERNEL_PHYSICAL_MAPPING_ADDRESS;
    }

    SimplePageTableAllocator::SimplePageTableAllocator(StaticMemoryAllocator& allocator)
        : m_pageTableAllocator(allocator), m_pml4base(AllocateClearPage()) {}

    void SimplePageTableAllocator::MapLocation(uint64_t location, uint64_t size, uint64_t virtualLocationBase) {
        size_t mappings = size / SINGLE_TABLE_MAPPING_SIZE;
        if (size % SINGLE_TABLE_MAPPING_SIZE != 0) {
            mappings++;
        }

        for (uint64_t i = 0; i < mappings; i++) {
            const uint64_t virtualAddr  = virtualLocationBase + i * SINGLE_TABLE_MAPPING_SIZE;
            const uint64_t physicalAddr = static_cast<uint64_t>(location) + i * SINGLE_TABLE_MAPPING_SIZE;

            MapSingleTable(virtualAddr, physicalAddr);
        }
    }

    void SimplePageTableAllocator::Map2MbPage(uint64_t physicalAddress, uint64_t virtualAddress) {
        F_ASSERT((virtualAddress % PAGE_DIRECTORY_ENTRY_SIZE) == 0, "Virtual address not aligned");
        F_ASSERT((physicalAddress % PAGE_DIRECTORY_ENTRY_SIZE) == 0, "Physical address not aligned");

        void* directory = GetPageStructure(m_pml4base, virtualAddress, 4, 2);
        auto* entries   = static_cast<uint64_t*>(directory);

        entries[(virtualAddress >> 21) & 0x1FF] = reinterpret_cast<uint64_t>(physicalAddress) | 0b10000011;
    }

    void* SimplePageTableAllocator::GetPml4Base() const {
        return m_pml4base;
    }

    void* SimplePageTableAllocator::AllocateClearPage() {
        void* page = m_pageTableAllocator.Allocate(PAGE_SIZE, PAGE_SIZE);
        Stdlib::Memory::SizedBuffer<uint8_t> buffer{reinterpret_cast<uint8_t*>(page), PAGE_SIZE};
        Stdlib::Memory::Set<uint8_t>(buffer, 0);
        return page;
    }

    void* SimplePageTableAllocator::GetPageStructure(
        void* currentBase, uint64_t virtualAddress, unsigned int level, unsigned int target) {
        F_ASSERT(((reinterpret_cast<uintptr_t>(currentBase) % PAGE_SIZE) == 0), "virtual address not page aligned");
        if (level == target) {
            return currentBase;
        }

        const unsigned int currentIndex = (virtualAddress >> (12 + (level - 1) * 9)) & 0x1FF;

        auto* entries = static_cast<uint64_t*>(currentBase);

        if ((entries[currentIndex] & 1) == 0) {
            // Flags 0b11 - supervisor only, read/write, present
            void* entry           = AllocateClearPage();
            entries[currentIndex] = reinterpret_cast<uint64_t>(entry) | 0b11;
        }

        void* nextBase = reinterpret_cast<void*>(entries[currentIndex] & 0xFFFFFFF000);

        return GetPageStructure(nextBase, virtualAddress, level - 1, target);
    }

    void SimplePageTableAllocator::MapSingleTable(uint64_t virtualAddress, uint64_t physicalAddress) {
        F_ASSERT((virtualAddress % SINGLE_TABLE_MAPPING_SIZE) == 0, "Virtual address not aligned");
        F_ASSERT((physicalAddress % PAGE_SIZE) == 0, "Physical address not aligned");

        FB_LOG_DEBUG_F("Maping table %016llx to %016llx", virtualAddress, physicalAddress);

        void* table        = GetPageStructure(m_pml4base, virtualAddress, 4, 1);
        auto* tableEntries = static_cast<uint64_t*>(table);

        for (size_t i = 0; i < ENTRIES_PER_PAGE; i++) {
            // Flags 0b11 - supervisor only, read/write, present
            tableEntries[i] = ((reinterpret_cast<uint64_t>(physicalAddress) + (i * PAGE_SIZE))) | 0b11;
        }
    }

}  // namespace FunnyOS::Bootloader32
