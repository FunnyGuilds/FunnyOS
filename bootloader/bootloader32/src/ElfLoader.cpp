#include "ElfLoader.hpp"

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include "Logging.hpp"

namespace FunnyOS::Bootloader32 {
    namespace {
        constexpr uint64_t ELF_MAGIC         = 0x464C457F;
        constexpr uint64_t ELF_FORMAT_64     = 2;
        constexpr uint64_t ELF_ENDIAN_LITTLE = 1;

    }  // namespace

    ElfLoader::ElfLoader(Misc::MemoryAllocator::StaticMemoryAllocator& memoryAllocator, FileLoader& fileLoader)
        : m_memoryAllocator(memoryAllocator), m_fileLoader(fileLoader) {}

    Stdlib::Memory::SizedBuffer<void> ElfLoader::LoadRegularFile(const char* file) {
        FB_LOG_INFO_F("Loading file: %s", file);

        m_fileLoader.OpenFile(file);
        void* memory = m_memoryAllocator.Allocate(m_fileLoader.GetFile().size, 1);
        m_fileLoader.LoadCurrentFile(memory);
        return {memory, m_fileLoader.GetFile().size};
    }

    ElfFileInfo ElfLoader::LoadElfFile(const void* file) {
        const auto* header = static_cast<const ElfHeader*>(file);

        ElfFileInfo elfFileInfo{};

        if (header->Magic != ELF_MAGIC) {
            F_ERROR_WITH_MESSAGE(FileLoadingFailure, "Failed to load file, invalid magic.");
        }

        if (header->Size != ELF_FORMAT_64) {
            F_ERROR_WITH_MESSAGE(FileLoadingFailure, "Failed to load file, invalid format.");
        }

        if (header->Endianness != ELF_ENDIAN_LITTLE) {
            F_ERROR_WITH_MESSAGE(FileLoadingFailure, "Failed to load file, invalid endian.");
        }

#ifdef F_DEBUG
        PostElfDebugLog("\tMagic = %d", header->Magic);
        PostElfDebugLog("\tSize = %d", header->Size);
        PostElfDebugLog("\tEndianness = %d", header->Endianness);
        PostElfDebugLog("\tABI = %d", header->ABI);
        PostElfDebugLog("\tAbiVersion = %d", header->AbiVersion);
        PostElfDebugLog("\tType = %d", header->Type);
        PostElfDebugLog("\tArchitecture = %d", header->Architecture);
        PostElfDebugLog("\tVersion = %d", header->Version);
        PostElfDebugLog("\tEntryPoint = %08llx", header->EntryPoint);
        PostElfDebugLog("\tPhOffset = %08llx", header->PhOffset);
        PostElfDebugLog("\tShOffset = %08llx", header->ShOffset);
        PostElfDebugLog("\tShOffset = %x", header->Flags);
        PostElfDebugLog("\tEhSize = %d", header->EhSize);
        PostElfDebugLog("\tPhEntSize = %d", header->PhEntSize);
        PostElfDebugLog("\tPhNum = %d", header->PhNum);
        PostElfDebugLog("\tShEntSize = %d", header->ShEntSize);
        PostElfDebugLog("\tShNum = %d", header->ShNum);
        PostElfDebugLog("\tShStrIndex = %d", header->ShStrIndex);
#endif

        const auto* phHeaderBase = reinterpret_cast<const uint8_t*>(header) + header->PhOffset;
        const auto* shHeaderBase = reinterpret_cast<const uint8_t*>(header) + header->ShOffset;

        // Find strtab
        const void* strtab = nullptr;

        for (size_t i = 0; i < header->ShNum; i++) {
            const auto* shHeader = reinterpret_cast<const SectionHeader*>(shHeaderBase + header->ShEntSize * i);
            if (shHeader->Type == SHType::SHT_STRTAB) {
                strtab = reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(file) + shHeader->FileOffset);
            }
        }

        // Find base address
        elfFileInfo.VirtualLocationBase = Stdlib::NumeralTraits::Info<uint64_t>::MaximumValue;

        for (size_t i = 0; i < header->ShNum; i++) {
            const auto* shHeader = reinterpret_cast<const SectionHeader*>(shHeaderBase + header->ShEntSize * i);
            PostElfDebugLog("\tSH header %u", i);

#ifdef F_DEBUG
            char name[128]                          = "No strtab";
            Stdlib::String::StringBuffer nameBuffer = {name, F_SIZEOF_BUFFER(name)};

            if (strtab != nullptr) {
                name[0] = 0;
                Stdlib::String::Append(nameBuffer, reinterpret_cast<const char*>(strtab) + shHeader->NameOffset);
            }

            PostElfDebugLog("\t\tNameOffset = %x", shHeader->NameOffset);
            PostElfDebugLog("\t\tName = %s", name);
            PostElfDebugLog("\t\tType = %x", shHeader->Type);
            PostElfDebugLog("\t\tFlags = %llx", shHeader->Flags);
            PostElfDebugLog("\t\tVirtualAddress = %llx", shHeader->VirtualAddress);
            PostElfDebugLog("\t\tFileOffset = %llx", shHeader->FileOffset);
            PostElfDebugLog("\t\tFileSize = %llx", shHeader->FileSize);
            PostElfDebugLog("\t\tLink = %x", shHeader->Link);
            PostElfDebugLog("\t\tInfo = %x", shHeader->Info);
            PostElfDebugLog("\t\tAlignment = %llx", shHeader->Alignment);
            PostElfDebugLog("\t\tEntrySize = %llx", shHeader->EntrySize);
#endif

            if ((shHeader->Flags & SHFlags::SHF_ALLOC) == 0) {
                continue;
            }

            elfFileInfo.VirtualLocationBase = Stdlib::Min(elfFileInfo.VirtualLocationBase, shHeader->VirtualAddress);
        }

        // Find memory top
        uint64_t virtualMemoryTop = 0;

        for (size_t i = 0; i < header->PhNum; i++) {
            const auto* phHeader = reinterpret_cast<const ProgramHeader*>(phHeaderBase + header->PhEntSize * i);

#ifdef F_DEBUG
            PostElfDebugLog("\tPH header %u", i);
            PostElfDebugLog("\t\tType = %x", phHeader->Type);
            PostElfDebugLog("\t\tFlags = %x", phHeader->Flags);
            PostElfDebugLog("\t\tImageOffset = %llx", phHeader->ImageOffset);
            PostElfDebugLog("\t\tVirtualAddress = %llx", phHeader->VirtualAddress);
            PostElfDebugLog("\t\tPhysicalAddress = %llx", phHeader->PhysicalAddress);
            PostElfDebugLog("\t\tSizeInFile = %llx", phHeader->SizeInFile);
            PostElfDebugLog("\t\tSizeInMemory = %llx", phHeader->SizeInMemory);
            PostElfDebugLog("\t\tAlignment = %llx", phHeader->Alignment);
#endif

            if (phHeader->VirtualAddress < elfFileInfo.VirtualLocationBase) {
                continue;
            }

            virtualMemoryTop = Stdlib::Max(virtualMemoryTop, phHeader->VirtualAddress + phHeader->SizeInMemory);
        }

        elfFileInfo.TotalMemorySize   = virtualMemoryTop - elfFileInfo.VirtualLocationBase;
        elfFileInfo.EntryPointVirtual = header->EntryPoint;

        void* loadedFile = m_memoryAllocator.Allocate(elfFileInfo.TotalMemorySize, 0x1000);

        elfFileInfo.PhysicalLocationBase = reinterpret_cast<uint64_t>(loadedFile);

#ifdef F_DEBUG
        PostElfDebugLog("\tVirtualLocationBase = %016llx", elfFileInfo.VirtualLocationBase);
        PostElfDebugLog("\tPhysicalLocationBase = %016llx", elfFileInfo.PhysicalLocationBase);
        PostElfDebugLog("\tTotalMemorySize = %llx", elfFileInfo.TotalMemorySize);
        PostElfDebugLog("\tEntryPointVirtual = %llx", elfFileInfo.EntryPointVirtual);
#endif

        // Load segments
        for (size_t i = 0; i < header->PhNum; i++) {
            const auto* phHeader = reinterpret_cast<const ProgramHeader*>(phHeaderBase + header->PhEntSize * i);

            if (phHeader->Type != PHType::PT_LOAD) {
                continue;
            }

            if (phHeader->VirtualAddress < elfFileInfo.VirtualLocationBase) {
                continue;
            }

            uint8_t* physicalAddress = reinterpret_cast<uint8_t*>(phHeader->VirtualAddress) -
                                       elfFileInfo.VirtualLocationBase + elfFileInfo.PhysicalLocationBase;
            const uint8_t* imageAddress = reinterpret_cast<const uint8_t*>(file) + phHeader->ImageOffset;

            // Copy segment to memory
            Stdlib::Memory::Copy(physicalAddress, imageAddress, phHeader->SizeInFile);

            // Zero out the rest
            if (phHeader->SizeInFile < phHeader->SizeInMemory) {
                uint8_t* zeroMemoryStart = physicalAddress + phHeader->SizeInMemory;

                const auto zeroSize = static_cast<size_t>(phHeader->SizeInMemory - phHeader->SizeInFile);
                Stdlib::Memory::SizedBuffer<uint8_t> zeroBuffer = {zeroMemoryStart, zeroSize};
                Stdlib::Memory::Set<uint8_t>(zeroBuffer, 0);
            }
        }

        return elfFileInfo;
    }  // namespace FunnyOS::Bootloader32

    void ElfLoader::PostElfDebugLog(const char* format, ...) {
        if (!s_debugElfs) {
            return;
        }

        va_list args;
        va_start(args, format);
        Logging::GetLogger().PostMessage(Stdlib::LogLevel::Debug, format, &args);
        va_end(args);
    }

    bool ElfLoader::s_debugElfs = false;

    bool ElfLoader::IsDebugElfs() {
        return s_debugElfs;
    }

    void ElfLoader::SetDebugElfs(bool debugElfs) {
        s_debugElfs = debugElfs;
    }

}  // namespace FunnyOS::Bootloader32