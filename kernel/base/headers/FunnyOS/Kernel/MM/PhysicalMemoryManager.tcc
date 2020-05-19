#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_HPP
#error "Include PhysicalMemoryManager.hpp instead"
#endif

#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_TCC
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_TCC

namespace FunnyOS::Kernel::MM {
    inline void* PhysicalAddressToPointer(physicaladdress_t address) {
        // The entire physical address space is mapped to virtual memory, starting at offset
        // defined by F_KERNEL_PHYSICAL_MAPPING_ADDRESS
        return reinterpret_cast<void*>(F_KERNEL_PHYSICAL_MAPPING_ADDRESS + address);
    }

    template <typename T>
    inline T* PhysicalAddressToPointer(physicaladdress_t address) {
        return static_cast<T*>(PhysicalAddressToPointer(address));
    }

    inline physicaladdress_t AlignToPage(physicaladdress_t address) {
        if (address % PAGE_SIZE == 0) {
            // Already aligned
            return address;
        }

        return address + (PAGE_SIZE - (address % PAGE_SIZE));
    }
}  // namespace FunnyOS::Kernel::MM

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_PHYSICALMEMORYMANAGER_TCC
