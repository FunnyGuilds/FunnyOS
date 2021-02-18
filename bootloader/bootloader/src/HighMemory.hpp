#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP

#include <FunnyOS/Bootparams/MemoryMapDescription.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticFragmentedMemoryAllocator.hpp>

namespace FunnyOS::Bootloader64 {

    /**
     * Returns the current memory map.
     *
     * @return the current memory map.
     */
    Stdlib::Vector<Bootparams::MemoryRegion>& GetMemoryMap();

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
     * Merge adjacent memory regions of the same types into bigger regions.
     */
    void MergeAdjacentMemoryRegions();

    void InitializeHighMemory();

    Misc::MemoryAllocator::StaticFragmentedMemoryAllocator& GetHighMemoryAllocator();

    size_t GetTotalHighMemoryAvailable();

    /**
     * Dumps the memory map to the standard output. Works only in debug mode.
     */
    void DumpMemoryMap();
}

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_HIGHMEMORY_HPP
