#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_MEMORYMAP_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_MEMORYMAP_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>

namespace FunnyOS::Bootloader32 {
    int CreateMemoryMap(Bootparams::MemoryMapDescription& map);

    Bootparams::MemoryMapEntry FindBiggestUsableMemoryEntry(const Bootparams::MemoryMapDescription& memoryMap);
}

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_MEMORYMAP_HPP
