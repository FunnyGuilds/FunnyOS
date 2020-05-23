#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP

#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>
#include "FileLoader.hpp"

namespace FunnyOS::Bootloader32 {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(FileLoadingFailure);

    /**
     * Represents an ELF object type
     */
    enum class ElfObjectType : uint16_t {
        ET_NONE   = 0x00,
        ET_REL    = 0x01,
        ET_EXEC   = 0x02,
        ET_DYN    = 0x03,
        ET_CORE   = 0x04,
        ET_LOOS   = 0xfe00,
        ET_HIOS   = 0xfeff,
        ET_LOPROC = 0xff00,
        ET_HIPROC = 0xffff,
    };

    /**
     * Represents an ELF file header.
     */
    struct ElfHeader {
        uint32_t Magic;
        uint8_t Size;
        uint8_t Endianness;
        uint8_t Version;
        uint8_t ABI;
        uint8_t AbiVersion;
        uint8_t Padding[7];
        ElfObjectType Type;
        uint16_t Architecture;
        uint32_t Version2;
        uint64_t EntryPoint;
        uint64_t PhOffset;
        uint64_t ShOffset;
        uint32_t Flags;
        uint16_t EhSize;
        uint16_t PhEntSize;
        uint16_t PhNum;
        uint16_t ShEntSize;
        uint16_t ShNum;
        uint16_t ShStrIndex;
    } F_DONT_ALIGN;

    /**
     * Represents a type of a program header.
     */
    enum class PHType : uint32_t {
        PT_NULL    = 0x00000000,
        PT_LOAD    = 0x00000001,
        PT_DYNAMIC = 0x00000002,
        PT_INTERP  = 0x00000003,
        PT_NOTE    = 0x00000004,
        PT_SHLIB   = 0x00000005,
        PT_PHDR    = 0x00000006,
        PT_TLS     = 0x00000007,
        PT_LOOS    = 0x60000000,
        PT_HIOS    = 0x6FFFFFFF,
        PT_LOPROC  = 0x70000000,
        PT_HIPROC  = 0x7FFFFFFF,
    };

    /**
     * An ELF program header.
     */
    struct ProgramHeader {
        PHType Type;
        uint32_t Flags;
        uint64_t ImageOffset;
        uint64_t VirtualAddress;
        uint64_t PhysicalAddress;
        uint64_t SizeInFile;
        uint64_t SizeInMemory;
        uint64_t Alignment;
    } F_DONT_ALIGN;

    /**
     * Represents a type of a section header.
     */
    enum class SHType : uint32_t {
        SHT_NULL          = 0x0,
        SHT_PROGBITS      = 0x1,
        SHT_SYMTAB        = 0x2,
        SHT_STRTAB        = 0x3,
        SHT_RELA          = 0x4,
        SHT_HASH          = 0x5,
        SHT_DYNAMIC       = 0x6,
        SHT_NOTE          = 0x7,
        SHT_NOBITS        = 0x8,
        SHT_REL           = 0x9,
        SHT_SHLIB         = 0x0A,
        SHT_DYNSYM        = 0x0B,
        SHT_INIT_ARRAY    = 0x0E,
        SHT_FINI_ARRAY    = 0x0F,
        SHT_PREINIT_ARRAY = 0x10,
        SHT_GROUP         = 0x11,
        SHT_SYMTAB_SHNDX  = 0x12,
        SHT_NUM           = 0x13,
    };

    namespace SHFlags {
        constexpr uint32_t SHF_WRITE            = 0x1;
        constexpr uint32_t SHF_ALLOC            = 0x2;
        constexpr uint32_t SHF_EXECINSTR        = 0x4;
        constexpr uint32_t SHF_MERGE            = 0x10;
        constexpr uint32_t SHF_STRINGS          = 0x20;
        constexpr uint32_t SHF_INFO_LINK        = 0x40;
        constexpr uint32_t SHF_LINK_ORDER       = 0x80;
        constexpr uint32_t SHF_OS_NONCONFORMING = 0x100;
        constexpr uint32_t SHF_GROUP            = 0x200;
        constexpr uint32_t SHF_TLS              = 0x400;
        constexpr uint32_t SHF_MASKOS           = 0x0ff00000;
        constexpr uint32_t SHF_MASKPROC         = 0xf0000000;
        constexpr uint32_t SHF_ORDERED          = 0x4000000;
        constexpr uint32_t SHF_EXCLUDE          = 0x8000000;
    }  // namespace SHFlags

    /**
     * Represents an ELF section header.
     */
    struct SectionHeader {
        uint32_t NameOffset;
        SHType Type;
        uint64_t Flags;
        uint64_t VirtualAddress;
        uint64_t FileOffset;
        uint64_t FileSize;
        uint32_t Link;
        uint32_t Info;
        uint64_t Alignment;
        uint64_t EntrySize;
    } F_DONT_ALIGN;

    /**
     * Contains additional information gathered while loading an file.
     */
    struct ElfFileInfo {
        /**
         * Virtual location at where the .elf file should be put on.
         */
        uint64_t VirtualLocationBase;

        /**
         * Physical location at where the .elf file was loaded at.
         */
        uint64_t PhysicalLocationBase;

        /**
         * Total size of the executable in memory.
         */
        uint64_t TotalMemorySize;

        /**
         * Virtual address of the .elf entry point.
         */
        uint64_t EntryPointVirtual;
    };

    /**
     * Helper for loading files and parsing .elf files.
     */
    class ElfLoader {
       public:
        /**
         * Constructs a new ElfLoader
         *
         * @param memoryAllocator allocator that will be used to allocate memory for the loaded files.
         * @param fileLoader file loader to be used to load files.
         */
        ElfLoader(Misc::MemoryAllocator::StaticMemoryAllocator& memoryAllocator, FileLoader& fileLoader);

       public:
        /**
         * Loads a regular file [file] to memory.
         *
         * @param file path to the file
         * @return pointer to where the file was loaded to.
         */
        Stdlib::Memory::SizedBuffer<void> LoadRegularFile(const char* file);

        /**
         * Parses and loads the given elf file loaded at [file].
         *
         * @param file raw .elf file in memory to load.
         * @return the [ElfFileInfo] struct for the loaded file.
         */
        ElfFileInfo LoadElfFile(const void* file);

       public:
        /**
         * @return whether or not .elf debugging is enabled.
         */
        static bool IsDebugElfs();

        /**
         * Sets whether or not .elf debugging is enabled.
         *
         * @param debugElfs whether or not .elf debugging is enabled
         */
        static void SetDebugElfs(bool debugElfs);

       private:
        static void PostElfDebugLog(const char* format, ...);

       private:
        static bool s_debugElfs;
        Misc::MemoryAllocator::StaticMemoryAllocator& m_memoryAllocator;
        FileLoader& m_fileLoader;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_ELFLOADER_HPP
