#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP

#define FOS_QUICKFAT_NO_NUMERAL_TYPES

#include <FunnyOS/Stdlib/String.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/Drive/DriveInterface.hpp>
#include <FunnyOS/QuickFat/QuickFat.h>

namespace FunnyOS::Bootloader32 {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(FileLoadException);

    /**
     * Simple file loader capable of reading files from FAT32 partitions.
     */
    class FileLoader {
       public:
        /**
         * Constructs new FileLoader
         *
         * @param driveInterface drive interface to be used for reads
         * @param partition partition number (starting from one) which this loader should read from.
         */
        FileLoader(HW::IDriveInterface& driveInterface, uint8_t partition);

       public:
        /**
         * Opens a file.
         * The opened file can be accessed via [GetFile()]
         *
         * @param path path to the file
         * @throws FileLoadException when file is not found
         */
        void OpenFile(const char* path);

        /**
         * Loads the current file (previously opened by OpenFile) into memory.
         *
         * @param destination memory location at where to load the file to.
         */
        void LoadCurrentFile(void* destination);

        /**
         * @return the QuickFat_Context used by this loader.
         */
        [[nodiscard]] const QuickFat_Context& GetQuickFatContext() const;

        /**
         *
         * @return
         */
        [[nodiscard]] const QuickFat_File& GetFile() const;

       public:
        /**
         * @return whether or not the disk reads debugging is c.
         */
        [[nodiscard]] static bool IsDebugReads();

        /**
         * Sets whether or not the disk reads debugging is enabled.
         *
         * @param debugReads whether or not the disk reads debugging is enabled.
         */
        static void SetDebugReads(bool debugReads);

       private:
        static int QuickFatRead(void* data, uint32_t lba, uint32_t count, uint8_t* out);

        void DieOnError(const char* error, int code);

       private:
        static bool s_debugReads;

        Stdlib::String::StringBuffer m_bufferInfo;
        HW::IDriveInterface& m_driveInterface;
        char m_errorBuffer[256] = {0};
        QuickFat_Context m_quickFatContext{};
        QuickFat_File m_file{};
        const char* m_fileName;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_FILELOADER_HPP
