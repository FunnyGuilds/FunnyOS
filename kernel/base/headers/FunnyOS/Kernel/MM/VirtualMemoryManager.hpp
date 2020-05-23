#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP

#include <FunnyOS/Kernel/Config.hpp>
#include "PhysicalMemoryManager.hpp"

namespace FunnyOS::Kernel {
    class Kernel64;

    namespace MM {
        class PhysicalMemoryManager;

        /**
         * Exception thrown by MapPage methods when called with incorrect arguments.
         */
        F_TRIVIAL_EXCEPTION_WITH_MESSAGE(PageSetupFailure);

        enum PageAttributes {
            /**
             * Page is writable.
             */
            PAGE_WRITABLE = (1 << 0),

            /**
             * Page is executable.
             *
             * If Nx bit is not supported (VirtualMemoryManager::NxSupported())
             */
            PAGE_EXECUTABLE = (1 << 1),

            /**
             * If set page is only accessible from supervisor (rings 0, 1 and 2) and is marked global (shared between
             * all namespaces).
             */
            PAGE_KERNEL = (1 << 2)
        };

        /**
         * VirtualMemoryManager manages the virtual to physical memory mappings.
         */
        class VirtualMemoryManager {
           public:
            /**
             * Initializes kernel page tables.
             *
             * This maps the entire physical memory and the kernel data to a higher half address.
             */
            void InitializePageTables();

            /**
             * Flushes the entire translation lookaside buffer (TLB).
             */
            void FlushTLB();

            /**
             * Maps a single 4 KB page.
             *
             * @param physicalAddress physical address to be mapped. Must be aligned to 4 KB
             * @param virtualAddress virtual address to be mapped. Must be aligned to 4 KB
             * @param attributes attributes of that page (see PageAttributes)
             */
            void Map4KbPage(physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes);

            /**
             * Maps a single 2 MB page.
             *
             * @param physicalAddress physical address to be mapped. Must be aligned to 2 MB
             * @param virtualAddress virtual address to be mapped. Must be aligned to 2 MB
             * @param attributes attributes of that page (see PageAttributes)
             */
            void Map2MbPage(physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes);

            /**
             * Maps a single 1 GB page.
             * If 1 GB pages are not supported this function will fall back to mapping ()
             *
             * @param physicalAddress physical address to be mapped. Must be aligned to 1 GB
             * @param virtualAddress virtual address to be mapped. Must be aligned to 1 GB
             * @param attributes attributes of that page (see PageAttributes)
             */
            void Map1GbPage(physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes);

            /**
             * Checks whether or not the NX (no-execute) bit is supported by this CPU.
             *
             * @return whether or not the NX no-execute bit is supported.
             */
            static bool NxSupported();

            /**
             * Checks whether or not the 1 GB page size is supported by this CPU.
             *
             * @return  whether or not the 1 GB page size is supported
             */
            static bool PDPE1GB_Supported();

           private:
            VirtualMemoryManager(PhysicalMemoryManager& pmm);

            /**
             * Maps a virtual address to a physical address using a single page.
             *
             * Both [physicalAddress] and [virtualAddress] must be aligned by the caller to either 4 KB, 2 MB or 1GB
             * (based on the size of the mapping specified by [structureLevel.).
             *
             * @param physicalAddress physical address to be mapped. Must be aligned.
             * @param virtualAddress virtual address to be mapped. Must be aligned.
             * @param structureLevel structure level at which to map the page. This also describe the size of the
             * mapping. (1 - page table, 4 KB mapping, 2 - page directory, 2 MB mapping, 3 - PDPE, 1 GB mapping).
             * @param attributes attributes of that page (see PageAttributes)
             * @param skipChecks if [true] no checks validating the page structure consistency will be done, allowing to
             * map smaller pages into bigger pages. Should never be used for things other than emulating bigger pages by
             * smaller pages (i.e. pdpe1gb support).
             *
             * @return pointer the newly created entry, can be used to set additional PageStructureFlags
             */
            uint64_t* MapAddress(
                physicaladdress_t physicalAddress, uintptr_t virtualAddress, unsigned int structureLevel,
                PageAttributes attributes, bool skipChecks);

            /**
             * Allocate a one, physical 4 KB page and set all the bytes in to 0.
             *
             * @return pointer to the beginning of the newly allocated page
             */
            physicaladdress_t AllocatePage();

            /**
             * Gets a pointer to a paging structure (i.e. to a page table or a page directory) by traversing the
             * structure recursively and allocating all the required entry.
             *
             * For example: if page table for address X is being accessed everything higher up in the structure that
             * would point to this page table (page directory, pdpt, pml4) is also allocated.
             *
             * @param virtualAddress virtual address to get the entry for
             * @param target the target entry level  (1 - page table, 2 - page directory, 3 - PDPE, 4 - PML5).
             * @param skipChecks if [true] no checks validating the page structure consistency will be done, allowing to
             * map smaller pages into bigger pages. Should never be used for things other than emulating bigger pages by
             * smaller pages (i.e. pdpe1gb support).
             *
             * @return address to the base of the requested paging structure.
             */
            physicaladdress_t GetPageStructure(uintptr_t virtualAddress, unsigned int target, bool skipChecks);

            /**
             * Gets a pointer to a paging structure (i.e. to a page table or a page directory) by traversing the
             * structure recursively and allocating all the required entry.
             *
             * For example: if page table for address X is being accessed everything higher up in the structure that
             * would point to this page table (page directory, pdpt, pml4) is also allocated.
             *
             * @param current current structure base, should be the page table base for the first call
             * @param virtualAddress virtual address to get the entry for
             * @param level current level for the recursive call. Should always be 4 for 4-level paging.
             * @param target the target entry level  (1 - page table, 2 - page directory, 3 - PDPE, 4 - PML5).
             * @param skipChecks if [true] no checks validating the page structure consistency will be done, allowing to
             * map smaller pages into bigger pages. Should never be used for things other than emulating bigger pages by
             * smaller pages (i.e. pdpe1gb support).
             *
             * @return address to the base of the requested paging structure.
             */
            physicaladdress_t GetPageStructureRecursively(
                physicaladdress_t current, uintptr_t virtualAddress, unsigned int level, unsigned int target,
                bool skipChecks);

            friend class ::FunnyOS::Kernel::Kernel64;

           private:
            physicaladdress_t m_pageTableBase;
            PhysicalMemoryManager& m_pmm;
        };
    }  // namespace MM

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP
