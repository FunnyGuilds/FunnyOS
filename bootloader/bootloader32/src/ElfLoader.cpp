#include "ElfLoader.hpp"

namespace FunnyOS::Bootloader32 {
    namespace {
        constexpr size_t PAGE_ALIGNMENT = 0x1000;
    }

    ElfLoader::ElfLoader(Misc::MemoryAllocator::StaticMemoryAllocator& memoryAllocator, FileLoader& fileLoader)
        : m_memoryAllocator(memoryAllocator), m_fileLoader(fileLoader) {}

    void* ElfLoader::LoadRegularFile(const char* file) {
        m_fileLoader.OpenFile(file);
        void* memory = m_memoryAllocator.Allocate(m_fileLoader.GetFile().size, PAGE_ALIGNMENT);
        m_fileLoader.LoadCurrentFile(memory);
        return memory;
    }

}  // namespace FunnyOS::Bootloader32