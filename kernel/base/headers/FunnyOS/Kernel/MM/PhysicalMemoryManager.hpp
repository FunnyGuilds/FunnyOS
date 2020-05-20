#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/Vector.hpp>
#include <FunnyOS/Kernel/Config.hpp>

namespace FunnyOS::Kernel {
    class Kernel64;

    namespace MM {
        /**
         * Size of a single page in memory.
         */
        constexpr const size_t PAGE_SIZE = 0x1000;

        /**
         * Represents an integer type used for storing physical addresses.
         *
         * Physical addresses can be accessed via PhysicalAddressToPointer.
         */
        using physicaladdress_t = uint64_t;

        /**
         * Value representing a null physical address. (equivalent of a [nullptr] for pointers)
         */
        constexpr const physicaladdress_t NULL_ADDRESS = 0;

        /**
         * Converts a physical address to a pointer that points to a virtual memory that maps to that physical address.
         *
         * @param address physical address to get the pointer to
         * @return pointer pointing to virtual memory mapped to that address
         */
        inline void* PhysicalAddressToPointer(physicaladdress_t address);

        /**
         * Converts a physical address to a pointer that points to a virtual memory that maps to that physical address.
         *
         * @tparam T type of the pointer to return
         * @param address  physicaladdress to get the pointer to
         * @return pointer pointing to virtual memory mapped to that address
         */
        template <typename T>
        inline T* PhysicalAddressToPointer(physicaladdress_t address);

        /**
         * Aligns address to a [PAGE_SIZE], always upwards. Does nothing if the address is already aligned.
         *
         * Examples:
         *   0x1234 => 0x2000
         *   0x2000 -> 0x2000
         *   0x4001 -> 0x5000
         *
         * @param address address to be aligned
         * @return aligned address
         */
        inline physicaladdress_t AlignToPage(physicaladdress_t address);

        /**
         * Structure holding information about allocated pages.
         */
        struct PageBuffer {
            /**
             * Physical address of the fist allocated page.
             *
             * May be [NULL_ADDRESS] if this buffer does not point to valid page.
             */
            physicaladdress_t Start;

            /**
             * Amount of allocated, contiguous pages.
             */
            size_t Count;
        };

        /**
         * Memory chunk control block is a structure that is put at the beginning of every initialized MemoryRegion.
         *
         * It holds basic information about that MemoryRegion.
         */
        struct MemoryChunkControlBlock {
            /**
             * Pointer to where the allocation bit map starts.
             *
             * The size of the bitmap is equal to [PagesCount / 8] rounded up.
             */
            physicaladdress_t BitmapStart;

            /**
             * Total amount of memory pages that can be allocated or are allocated.
             */
            size_t AllocablePagesCount;

            /**
             * Total amount of memory pages that are free and can be allocated.
             */
            size_t FreePages;

            /**
             * Physical, absolute, memory location at where the first allocable page of this chunk begins.
             */
            physicaladdress_t FirstPageBegin;

            /**
             * How many of the first allocable pages are spanned by the control block and its allocation bitmap.
             */
            physicaladdress_t ControlBlockPageSpan;
        };

        /**
         * Describes a region in the physical memory.
         */
        struct MemoryRegion {
           public:
            /**
             * Type of the memory in this region.
             */
            Bootparams::MemoryMapEntryType RegionMemoryType;

            /**
             * Start of the region (inclusive)
             */
            physicaladdress_t RegionStart;

            /**
             * End of the region (exclusive)
             */
            physicaladdress_t RegionEnd;

            /**
             * Whether or not this region has any usability for the PhysicalMemoryManager.
             *
             * For a memory region to be usable it must be available or reclaimable.
             */
            bool IsUsable;

            /**
             * Was this memory region initialized (does it contain a MemoryChunkControlBlock).
             *
             * If a memory region is not initialized it may not be used for allocations.
             *
             * Every memory region that is marked as ready will be initialized on a InitializeMemoryRegions call.
             */
            bool IsInitialized;

            /**
             * Whether or not this region is ready to be initialized.
             */
            bool IsReady;

           public:
            /**
             * Gets the MemoryChunkControlBlock for this region.
             * The result of this call is undefined for any memory type other than Available.
             *
             * @return memory chunk control block for a region.
             */
            MemoryChunkControlBlock& GetControlBlock();

            /**
             * @return the total length (in bytes) of this chunk
             */
            uint64_t GetLength() const;
        };

        struct MemoryStatistics {
            /**
             * Amount of memory (in bytes) that is dedicated solely to holding MemoryChunkControlBlocks and memory
             * allocation bitmaps.
             * It cannot be repurposed and it is not counted as available memory in further statistics.
             */
            uint64_t ControlBlockWaste;

            /**
             * Amount of memory (in bytes) that is wasted and cannot be allocated due to not being aligned to a page
             * size.
             *
             * I.e. if memory region starts at 0x1234 and ends at 0x6789 the actual region used for allocation is only
             * 0x2000 to 0x6000. In this situation the first 0x234 and the last 0x789 bytes are wasted and never used.
             */
            uint64_t UnusableUnalignedMemory;

            /**
             * Amount of memory (in bytes) that cannot be used because it is dedicated to be used by real mode code and
             * holding bootloader's boot structures.
             *
             * It is always set to exactly 1 MB by physical memory manager.
             */
            uint64_t UnusableLowMemory;

            /**
             * Amount of memory (in bytes) that cannot be used because it is contained in chunks that are too small to
             * hold one control block + at least one single page of memory.
             *
             * For a region of physical to be used in allocation, its size must be at least [2 * PAGE_SIZE] bytes. Any
             * region smaller than this is counted towards this statistic and is wasted and never used.
             */
            uint64_t UnusableFragmentedMemory;

            /**
             * Total amount of memory (in bytes) that is taken by the kernel code, kernel stack and initial kernel heap.
             *
             * It cannot be repurposed.
             */
            uint64_t KernelImageSize;

            /**
             * Total amount of memory that is unusable now but may be reclaimed and turned into usable memory later.
             * (for example memory that holds ACPI tables or memory that holds temporary page tables prepared by the
             * bootloader)
             */
            uint64_t TotalReclaimableMemory;

            /**
             * Total memory (in bytes) that is available to be allocate via PhysicalMemoryManager.
             *
             * It is always divisible by [PAGE_SIZE]
             */
            uint64_t TotalAvailableMemory;

            /**
             * Total amount of memory that is physically available but is not going to be used due to its fragmentation
             * or alignment.
             *
             * The returned value is equal to [UnusableUnalignedMemory + UnusableLowMemory]
             *
             * @return total amount of wasted memory.
             */
            uint64_t GetTotalWastedMemory() const;

            /**
             * Total amount of memory that is physically available but is not going to be used due to holding internal
             * structures or being misaligned.
             *
             * The returned value is equal to [ControlBlockWaste + KernelImageSize + GetTotalWastedMemory()]
             *
             * @return total amount of unusable memory
             */
            uint64_t GetTotalUnusableMemory() const;
        };

        /**
         * Checks whether or not this memory type is available to use as is.
         */
        bool IsMemoryTypeAvailable(Bootparams::MemoryMapEntryType type);

        /**
         * Checks whether or not this memory type may be reclaimed and turned into available memory in the future.
         */
        bool IsMemoryTypeReclaimable(Bootparams::MemoryMapEntryType type);

        /**
         * Checks whether the memory map entry and index [entryIndex] is usable and is either available or reclaimable.
         */
        bool IsMemoryMapEntryUsable(const Bootparams::MemoryMapDescription& map, size_t entryIndex);

        /**
         * Checks whether physical address [address] is contained in region [region]
         */
        bool IsInRegion(physicaladdress_t address, const MemoryRegion& region);

        /**
         * Gets a wiegh of a memory type.
         * This weights describes precedence of a memory region when overlaping regions are removed by
         * RemoveOverlappingRegions.
         *
         * In case if there is a memory area that is contained at the same tame by two or more MemoryRegions, the region
         * with the highest weight will be the one that will get the sole ownership of that area. (In case of two
         * regions with the same weight containing the same the owner is choosen arbitrarily).
         */
        unsigned int GetMemoryTypeWeight(Bootparams::MemoryMapEntryType type);

        /**
         * Manager for physical memory. Responsible for keeping track of usable memory and allocating and freeing
         * physical memory pages.
         */
        class PhysicalMemoryManager {
           public:
            /**
             * Initializes this PhysicalMemoryManager with the given memory map.
             * The PhysicalMemoryManager will build a MemoryRegion map based on the given memory map and initialize
             * every entry below the 4GB memory boundary. To initialize entries above 4GB boundary ReclaimMemory must
             * be called with parameter [LongMemReclaimable].
             *
             * @param map initial memory map passed by the bootloader from E820 call.
             */
            void Initialize(const Bootparams::MemoryMapDescription& map);

            /**
             * Makes reclaimable memory of the given type [type] reclaimed, it is marked as ready to be used and
             * initialized.
             *
             * @param type type of memory to be reclaimed
             */
            void ReclaimMemory(Bootparams::MemoryMapEntryType type);

            /**
             * Allocates [pages] of subsequent memory pages.
             * Allocating [0] pages is permitted but the returned base address is an abstract memory location that may
             * not be accessed and may not be mapped to any physical memory but is valid to be used for FreePagesRaw
             * calls.
             *
             * @param pages number of pages to allocate
             * @return base address of the first page in the allocated block or [NULL_ADDRESS] if allocation failed due
             * to lack of available, free memory or due to there not being a free contiguous chunk of pages with size of
             * at least [pages].
             */
            physicaladdress_t AllocatePagesRaw(size_t pages);

            /**
             * Frees [pages] of subsequent pages starting from page that begings at address [base].
             *
             * @param base base of the first page to free, this address MUST be page aligned.
             * @param pages number of subsequent pages to free. [0] is a valid value and results in no operation
             */
            void FreePagesRaw(physicaladdress_t base, size_t pages);

            /**
             * Allocates a single physical frame.
             * Equivalent of calling [AllocatePagesRaw(1)]
             *
             * @return base address of the allocated page or [NULL_ADDRESS] if allocation failed due to lack of
             * available, free memory.
             */
            physicaladdress_t AllocatePage();

            /**
             * Frees a single physical frame.
             * Equivalent of calling [FreePagesRaw(1)]
             *
             * @param page base address of the page
             */
            void FreePage(physicaladdress_t page);

            /**
             * Allocates pages and puts the data into a PageBuffer.
             * See AllocatePagesRaw for more info.
             *
             * @param pages number of pages to allocate
             * @return PageBuffer with page base and amount of the allocated pages, if AllocatePagesRaw returned
             */
            PageBuffer AllocatePages(size_t pages);

            /**
             * Frees pages described by a PageBuffer.
             * See FreePagesRaw for more info.
             *
             * @param buffer buffer containing page base .
             */
            void FreePages(PageBuffer& buffer);

            /**
             * Get the highest addressable address in the physical memory.
             *
             * @return the highest addressable address in the physical memory.
             */
            physicaladdress_t GetPhysicalMemoryTop() const;

            /**
             * Gets the original memory map passed by the bootloader.
             * This memory map may contain overlapping areas.
             *
             * @return the original memory map
             */
            const Stdlib::Vector<Bootparams::MemoryMapEntry>& GetOriginalMemoryMap() const;

           private:
            PhysicalMemoryManager();

            /**
             * Find a MemoryRegion that contains the address [address]
             *
             * @param address address to search for
             *
             * @return MemoryRegion containing this address or empty optional if no region contains this address.
             */
            Stdlib::Optional<MemoryRegion> FindEntryForRegion(physicaladdress_t address);

            /**
             * Initializes every MemoryRegion marked as ready.
             *
             * It marks every region as initialized and sets up a control block and allocation bitmap for that region.
             */
            void InitializeMemoryRegions();

            /**
             * Removes any overlapping areas in memory regions using a weight system (every memory region has its own
             * weight based on its type (see GetMemoryTypeWeight)).
             *
             * If there is a conflicting and a part of memory is described by two or more memory regions, the region
             * with the highest chosen as the sole owner of a memory area.
             *`
             * Example:
             *  Memory before removing overlapping regions:
             *      +-----------------------------+                                +-----------------------------+
             *      + 0x1000 -> 0x2000. Weight: 3 +                                + 0x3000 -> 0x4000. Weight: 1 +
             *      +-----------------------------+                                +-----------------------------+
             *                            +---------------------------------------------+
             *                            +         0x1500 -> 0x3500. Weight: 2         +
             *                            +---------------------------------------------+
             *                            ^^^^^^^^^^                               ^^^^^^
             *                            ^ Overlapping area 1                     ^ Overlapping area 2
             *                            ^ Will be taken by block                 ^ Will be taken by block
             *                            ^ with weight 3                          ^ with weight 2
             *                            ^                                        ^
             *
             *  After:
             *   +-----------------------------+ +-----------------------------------+ +-----------------------------+
             *   + 0x1000 -> 0x2000. Weight: 3 + +    0x2000 -> 0x3500. Weight: 2    + + 0x3500 -> 0x4000. Weight: 1 +
             *   +-----------------------------+ +-----------------------------------+ +-----------------------------+
             *
             *  Resulting memory is guaranteed to have no overlapping memory regions.
             */
            void RemoveOverlappingRegions();

            /**
             * Removes all regions from the MemoryRegion list that are not marked as usable.
             */
            void ClearUnusableRegions();

            /**
             * Dumps a memory region map to the kernel debug output.
             *
             * Used for debugging.
             */
            void DumpRegionsToDebug();

            friend class ::FunnyOS::Kernel::Kernel64;

           private:
            Stdlib::Vector<Bootparams::MemoryMapEntry> m_memoryMap;
            Stdlib::Vector<MemoryRegion> m_memoryRegions;
            MemoryStatistics m_memoryStatistics;
            physicaladdress_t m_physicalMemoryTop;
        };
    }  // namespace MM

}  // namespace FunnyOS::Kernel

#include "PhysicalMemoryManager.tcc"
#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_HPP
