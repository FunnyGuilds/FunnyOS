#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP

#define FOS_QUICKFAT_NO_NUMERAL_TYPES

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/Drive/DriveInterface.hpp>
#include <FunnyOS/QuickFat/QuickFat.h>

namespace FunnyOS::Bootloader32 {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(FileLoadException);

    class FileLoader {
       public:
        FileLoader(HW::IDriveInterface& driveInterface, uint8_t partition);

       public:
        void OpenFile(const char* path);

        void LoadCurrentFile(void* destination);

        const QuickFat_Context& GetQuickFatContext() const;

        const QuickFat_File& GetFile() const;

       public:
        static bool IsDebugReads();

        static void SetDebugReads(bool debugReads);

       private:
        static int QuickFatRead(void* data, uint32_t lba, uint32_t count, uint8_t* out);

        void DieOnError(const char* error, int code);
       private:
        static bool s_debugReads;

        char m_errorBuffer[256];
        Stdlib::String::StringBuffer m_bufferInfo;
        HW::IDriveInterface& m_driveInterface;
        QuickFat_Context m_quickFatContext;
        QuickFat_File m_file;
        const char* m_fileName;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP
