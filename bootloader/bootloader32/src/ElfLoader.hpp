#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP

#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>
#include "FileLoader.hpp"

namespace FunnyOS::Bootloader32 {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(FileLoadingFailure);

    class ElfLoader {
       public:
        ElfLoader(Misc::MemoryAllocator::StaticMemoryAllocator& memoryAllocator, FileLoader& fileLoader);

       public:
        void* LoadRegularFile(const char* file);

       private:
        Misc::MemoryAllocator::StaticMemoryAllocator& m_memoryAllocator;
        FileLoader& m_fileLoader;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP
