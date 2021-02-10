#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP

#include <FunnyOS/Bootparams/MemoryMapDescription.hpp>

namespace FunnyOS::Bootloader64 {

    Bootparams::MemoryMapDescription& GetMemoryMapDescription();

    void AddMemoryMapEntry(Bootparams::MemoryMapEntry entry);

    void InitializeHighMemory();

    void DumpMemoryMap();
}

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP
