#include <FunnyOS/Kernel/MM/VirtualMemoryManager.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Stdlib/Math.hpp>
#include <FunnyOS/Hardware/CPU.hpp>
#include <FunnyOS/Kernel/Kernel.hpp>

#define VMM_PREFIX "VMM: "

namespace FunnyOS::Kernel::MM {
    namespace {
        constexpr const size_t PHYSICAL_ADDRESS_MASK_PAGE_TABLE = 0x000FFFFFFFFFF000;
        constexpr const size_t PHYSICAL_ADDRESS_MASK_PAGE_DIRECTORY = 0x000FFFFFFFF00000;
        constexpr const size_t PHYSICAL_ADDRESS_MASK_PAGE_DIRECTORY_POINTER_TABLE = 0x000FFFFFFFFC0000000;

        enum class PageStructureFlags : uint64_t {
            /**
             * Present bit - whether or not this page is loaded in physical memory.
             */
            Present = (1ULL << 0),

            /**
             * Read/write byte - if set read and write access is allowed to page, if not set only read access is
             * allowed.
             */
            ReadWrite = (1ULL << 1),

            /**
             * User/Supervisor bit - if set any code can access this page, if not set only CPL 0, 1 and 2 can access
             * this page.
             */
            UserSupervisor = (1ULL << 2),

            /**
             * Page-Level Writethrough bit - if set page has a writethrough caching policy, if cleared page has a
             * writeback caching policy
             */
            PageLevelWritethrough = (1ULL << 3),

            /**
             * Page-Level Cache Disable - if set the physical page is not cacheable, if clreared the page is cacheable.
             */
            PageLevelCacheDisable = (1ULL << 4),

            /**
             * Accessed bit - set by processor if the page was read or written from, never cleared by the processor.
             */
            Accessed = (1ULL << 5),

            /**
             * Dirty bit - only present in page tables entries. Set by the processor if the page was written to, never
             * cleared by the processor.
             */
            Dirty = (1ULL << 6),

            /**
             * Page size bit - In page directory entries if this bit is set the page size is 2 MB.  In page directory
             * pointer entries if this bit is set the page size is 1 GB (only if pdpe1gb feature is supported by the
             * CPU).
             */
            PageSize = (1ULL << 7),

            /**
             * Global bit - only present in page tables entries. If set the TLB entry of this page is not invalidated
             * when CR3 register is loaded.
             */
            Global = (1ULL << 8),

            /**
             * Allocate bit (custom) - Not available on page tables. If set this entry points to a valid structure. If
             * not set the physical address of this entry does not point to a valid page table structure.
             */
            ExAllocated = (1ULL << 9),

            /**
             * Emulate pdpe1gb (custom) - Available only on PDPT entries. If set this PDPE emulates a behaviour of 1 GB
             * pages using 512 2MB pages on CPUs that do not support pdpe1gb.
             */
            ExEmulatePdpe1Gb = (1ULL << 10),

            /**
             * No execute bit - only available if NoExecute feature is supported by the CPU. If this bit is set code
             * cannot be executed from this page.
             */
            NxBit = (1ULL << 63),

        };

        /**
         * Sets the pointer to PML4.
         *
         * @param base pointer to the PML4 sturcture.
         */
        inline void SetPageTableBase(physicaladdress_t base) {
            F_ASSERT((base % PAGE_SIZE) == 0, "page table base not aligned");
            FK_LOG_DEBUG_F(VMM_PREFIX "New page table base is 0x%016llx", base);

#ifdef __GNUC__
            asm("mov %0, %%cr3" ::"r"(base) : "memory");
#endif
        }

        /**
         * Enable the NX bit support.
         */
        inline void EnableNx() {
            using namespace HW::CPU;

            // Set NX bit in EFER MSR
            uint64_t efer = ReadMSR(MSR::EFER);
            efer |= static_cast<uint64_t>(EferBits::NXE);
            WriteMSR(MSR::EFER, efer);
        }

        /**
         * Sets a page structure entry attribuetes based on [attributes].
         *
         * @param entry pointer to that entry
         * @param hasNx is the NX bit support enabled
         * @param attributes see PageAttributes
         */
        inline void SetEntryAttributes(uint64_t* entry, bool hasNx, PageAttributes attributes) {
            *entry |= static_cast<uint64_t>(PageStructureFlags::Present);
            if ((attributes & PAGE_WRITABLE) != 0) {
                *entry |= static_cast<uint64_t>(PageStructureFlags::ReadWrite);
            }

            if ((attributes & PAGE_EXECUTABLE) == 0 && hasNx) {
                *entry |= static_cast<uint64_t>(PageStructureFlags::NxBit);
            }

            if ((attributes & PAGE_KERNEL)) {
                *entry |= static_cast<uint64_t>(PageStructureFlags::Global);
            } else {
                *entry |= static_cast<uint64_t>(PageStructureFlags::UserSupervisor);
            }
        }

    }  // namespace

    void VirtualMemoryManager::FlushTLB() {
#ifdef __GNUC__
        // Flush the TLB by reloading the CR3 register to the same value.
        asm volatile(
            "push %rax         \n"
            "mov %cr3, %rax    \n"
            "mov %rax, %cr3    \n"
            "pop %rax          \n");
#endif
    }

    void VirtualMemoryManager::InitializePageTables() {
        FK_LOG_INFO(VMM_PREFIX "Initializing ...");

        if (VirtualMemoryManager::NxSupported()) {
            FK_LOG_DEBUG(VMM_PREFIX "NX bit is supported, enabling... ");
            EnableNx();
        } else {
            FK_LOG_WARNING(VMM_PREFIX "NX bit is not available! All pages will be executable!");
        }

        if (VirtualMemoryManager::PDPE1GB_Supported()) {
            FK_LOG_DEBUG(VMM_PREFIX "pdpe1gb is supported. 1 GB pages will be used if possible. ");
        }

        m_pageTableBase = AllocatePage();

        // Amount of 1 GB pages needed to map the entire physical memory
        const auto physicalPages =
            Stdlib::Math::DivideRoundUp<physicaladdress_t>(m_pmm.GetPhysicalMemoryTop(), PAGE_SIZE_1GB);

        FK_LOG_DEBUG_F(
            VMM_PREFIX "Mapping %llu bytes of physical memory at virtual address 0x%016llx",
            physicalPages * PAGE_SIZE_1GB, F_KERNEL_PHYSICAL_MAPPING_ADDRESS);

        for (size_t i = 0; i < physicalPages; i++) {
            Map1GbPage(
                PAGE_SIZE_1GB * i, F_KERNEL_PHYSICAL_MAPPING_ADDRESS + PAGE_SIZE_1GB * i,
                static_cast<PageAttributes>(PAGE_WRITABLE | PAGE_KERNEL));
        }

        // Map kernel image
        auto kernelImage = Stdlib::Find(m_pmm.GetOriginalMemoryMap(), [](const Bootparams::MemoryMapEntry& entry) {
            return entry.Type == Bootparams::MemoryMapEntryType::KernelImage;
        });
        F_ASSERT(kernelImage != m_pmm.GetOriginalMemoryMap().End(), "no KernelImage in memory map");

        const physicaladdress_t kernelBase = kernelImage->BaseAddress;
        const size_t kernelLength = kernelImage->Length;
        const auto kernelPages = Stdlib::Math::DivideRoundUp<physicaladdress_t>(kernelLength, PAGE_SIZE);

        FK_LOG_DEBUG_F(
            VMM_PREFIX "Kernel physical base: 0x%08llx. Size: 0x%08llx bytes", kernelImage->BaseAddress,
            kernelImage->Length);

        FK_LOG_DEBUG_F(
            VMM_PREFIX "Mapping %llu bytes of kernel data at virtual address 0x%016llx", physicalPages * PAGE_SIZE,
            F_KERNEL_VIRTUAL_ADDRESS);

        for (size_t i = 0; i < kernelPages; i++) {
            Map4KbPage(
                kernelBase + i * PAGE_SIZE, F_KERNEL_VIRTUAL_ADDRESS + i * PAGE_SIZE,
                static_cast<PageAttributes>(PAGE_WRITABLE | PAGE_KERNEL | PAGE_EXECUTABLE));
        }

        SetPageTableBase(m_pageTableBase);
        FK_LOG_OK(VMM_PREFIX "Initialized!");
    }

    void VirtualMemoryManager::Map4KbPage(
        physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes) {
        if ((physicalAddress & PHYSICAL_ADDRESS_MASK_PAGE_TABLE) != physicalAddress) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Physical address not aligned to 4KB");
        }

        if ((virtualAddress % PAGE_SIZE) != 0) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Virtual address not aligned to 4KB");
        }

        MapAddress(physicalAddress, virtualAddress, 1, attributes, false);
    }

    void VirtualMemoryManager::Map2MbPage(
        physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes) {
        if ((physicalAddress & PHYSICAL_ADDRESS_MASK_PAGE_DIRECTORY) != physicalAddress) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Physical address not aligned to 2MB");
        }

        if ((virtualAddress % PAGE_SIZE_2MB) != 0) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Virtual address not aligned to 2 MB");
        }

        uint64_t* entry = MapAddress(physicalAddress, virtualAddress, 2, attributes, false);
        *entry |= static_cast<uint64_t>(PageStructureFlags::PageSize);
    }

    void VirtualMemoryManager::Map1GbPage(
        physicaladdress_t physicalAddress, uintptr_t virtualAddress, PageAttributes attributes) {
        if ((physicalAddress & PHYSICAL_ADDRESS_MASK_PAGE_DIRECTORY_POINTER_TABLE) != physicalAddress) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Physical address not aligned to 1GB");
        }

        if ((virtualAddress % PAGE_SIZE_1GB) != 0) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "Virtual address not aligned to 1 GB");
        }

        if (!VirtualMemoryManager::PDPE1GB_Supported()) {
            // Emulate 1GB via 2MB pages

            // Mark base entry
            auto base = GetPageStructure(virtualAddress, 3, false);
            uint64_t* baseEntry = reinterpret_cast<uint64_t*>(base) + ((virtualAddress >> 30) & 0x1FF);
            *baseEntry |= static_cast<uint64_t>(PageStructureFlags::ExEmulatePdpe1Gb);

            // Use 2 MB pages
            for (size_t i = 0; i < (PAGE_SIZE_1GB / PAGE_SIZE_2MB); i++) {
                const size_t offset = PAGE_SIZE_2MB * i;
                uint64_t* entry = MapAddress(physicalAddress + offset, virtualAddress + offset, 2, attributes, true);
                *entry |= static_cast<uint64_t>(PageStructureFlags::PageSize);
            }

            return;
        }

        uint64_t* entry = MapAddress(physicalAddress, virtualAddress, 3, attributes, false);
        *entry |= static_cast<uint64_t>(PageStructureFlags::PageSize);
    }

    bool VirtualMemoryManager::NxSupported() {
        static bool c_nxSupported =
            HW::CPU::GetExtendedFeatureBits() & static_cast<uint64_t>(HW::CPU::CPUIDExtendedFeatures::NX);

        return c_nxSupported;
    }

    bool VirtualMemoryManager::PDPE1GB_Supported() {
        static bool c_pdpe1gbSupported =
            HW::CPU::GetExtendedFeatureBits() & static_cast<uint64_t>(HW::CPU::CPUIDExtendedFeatures::PDPE1GB);

        return c_pdpe1gbSupported;
    }

    VirtualMemoryManager::VirtualMemoryManager(PhysicalMemoryManager& pmm) : m_pmm(pmm) {}

    uint64_t* VirtualMemoryManager::MapAddress(
        physicaladdress_t physicalAddress, uintptr_t virtualAddress, unsigned int structureLevel,
        PageAttributes attributes, bool skipChecks) {
        F_ASSERT(structureLevel >= 1 && structureLevel <= 3, "structureLevel invalid");

        if (structureLevel == 3 && !VirtualMemoryManager::PDPE1GB_Supported()) {
            F_ERROR_WITH_MESSAGE(PageSetupFailure, "structureLevel was set to 3 but pdpe1gb is not supported");
        }

        physicaladdress_t pageTable = GetPageStructure(virtualAddress, structureLevel, skipChecks);

        const size_t entryIndex = (virtualAddress >> (12 + ((structureLevel - 1) * 9))) & 0x1FF;
        uint64_t* entry = PhysicalAddressToPointer<uint64_t>(pageTable) + entryIndex;

        if ((*entry & static_cast<uint64_t>(PageStructureFlags::ExAllocated)) != 0) {
            F_ERROR_WITH_MESSAGE(
                PageSetupFailure,
                VMM_PREFIX "tried to map a big page when smaller pages are already allocated at that address");
        }

        *entry = physicalAddress;
        SetEntryAttributes(entry, VirtualMemoryManager::NxSupported(), attributes);
        return entry;
    }

    physicaladdress_t VirtualMemoryManager::AllocatePage() {
        physicaladdress_t base = m_pmm.AllocatePage();
        FK_PANIC_IF(base == NULL_ADDRESS, VMM_PREFIX "failed to allocate page table structure");

        auto* basePtr = MM::PhysicalAddressToPointer<uint8_t>(base);
        Stdlib::Memory::SizedBuffer<uint8_t> baseBuffer{basePtr, PAGE_SIZE};
        Stdlib::Memory::Set<uint8_t>(baseBuffer, 0);

        return base;
    }

    physicaladdress_t VirtualMemoryManager::GetPageStructure(
        uintptr_t virtualAddress, unsigned int target, bool skipChecks) {
        return GetPageStructureRecursively(m_pageTableBase, virtualAddress, 4, target, skipChecks);
    }

    physicaladdress_t VirtualMemoryManager::GetPageStructureRecursively(
        physicaladdress_t current, uintptr_t virtualAddress, unsigned int level, unsigned int target, bool skipChecks) {
        F_ASSERT((current % PAGE_SIZE) == 0, "virtual address not page aligned");
        if (level == target) {
            return current;
        }

        const unsigned int currentIndex = (virtualAddress >> (12 + (level - 1) * 9)) & 0x1FF;
        auto* entries = PhysicalAddressToPointer<uint64_t>(current);

        if (!skipChecks) {
            static constexpr const uint64_t c_restrictedFlags =
                static_cast<uint64_t>(PageStructureFlags::PageSize) |
                static_cast<uint64_t>(PageStructureFlags::ExEmulatePdpe1Gb);

            if ((entries[currentIndex] & c_restrictedFlags) != 0) {
                if (level == 3) {
                    F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "tried to map a 2MB page inside a 1GB page");
                } else {
                    F_ERROR_WITH_MESSAGE(PageSetupFailure, VMM_PREFIX "tried to map a 4KB page inside a 2MB page");
                }
            }
        }

        if ((entries[currentIndex] & static_cast<uint64_t>(PageStructureFlags::ExAllocated)) == 0) {
            physicaladdress_t entry = AllocatePage();
            entries[currentIndex] |= static_cast<uint64_t>(PageStructureFlags::ExAllocated);
            entries[currentIndex] |= static_cast<uint64_t>(PageStructureFlags::Present);
            entries[currentIndex] |= static_cast<uint64_t>(PageStructureFlags::ReadWrite);
            entries[currentIndex] |= (entry & 0x000FFFF000);
        }

        const auto nextBase = static_cast<physicaladdress_t>(entries[currentIndex] & PHYSICAL_ADDRESS_MASK_PAGE_TABLE);
        return GetPageStructureRecursively(nextBase, virtualAddress, level - 1, target, skipChecks);
    }

}  // namespace FunnyOS::Kernel::MM