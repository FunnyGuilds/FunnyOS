#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>

namespace FunnyOS::Bootloader32 {
    using Misc::MemoryAllocator::StaticMemoryAllocator;

    uint64_t GetKernelVirtualLocation();

    void* SetupInitialKernelPages(uintmax_t location, uintmax_t kernelSize, StaticMemoryAllocator& kernelAllocator);

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_PAGING_HPP
