#include "FileLoader.hpp"
#include "Logging.hpp"

namespace FunnyOS::Bootloader32 {

    FileLoader::FileLoader(HW::IDriveInterface& driveInterface, uint8_t partition)
        : m_bufferInfo({m_errorBuffer, sizeof(m_errorBuffer) / sizeof(m_errorBuffer[0])}),
          m_driveInterface(driveInterface),
          m_fileName("N/A") {
        QuickFat_initialization_data initData;
        initData.read_function = &FileLoader::QuickFatRead;
        initData.read_function_data = this;
        initData.partition_entry = partition;
        initData.sector_size = driveInterface.GetSectorSize();

        DieOnError("initializing the QuickFat context", quickfat_init_context(&m_quickFatContext, &initData));
    }

    void FileLoader::OpenFile(const char* path) {
        m_fileName = path;
        DieOnError("opening a file", quickfat_open_file(&m_quickFatContext, &m_file, path));
    }

    void FileLoader::LoadCurrentFile(void* destination) {
        DieOnError("reading a file", quickfat_read_file(&m_quickFatContext, &m_file, destination));
    }

    const QuickFat_Context& FileLoader::GetQuickFatContext() const {
        return m_quickFatContext;
    }

    const QuickFat_File& FileLoader::GetFile() const {
        return m_file;
    }

    int FileLoader::QuickFatRead(void* data, uint32_t lba, uint32_t count, uint8_t* out) {
        auto& interface = static_cast<FileLoader*>(data)->m_driveInterface;
        Stdlib::Memory::SizedBuffer<uint8_t> buffer{out, count * interface.GetSectorSize()};
        interface.ReadSectors(lba, count, buffer);
        return 0;
    }

    void FileLoader::DieOnError(const char* error, int code) {
        if (code == 0) {
            return;
        }

        Stdlib::String::Format(m_bufferInfo,
                               "\r\n\t\tQuickFat error 0x%08x while %s."
                               "\r\n\t\t(file: %s)",
                               code, error, m_fileName);
        throw FileLoadException(m_errorBuffer);
    }

}  // namespace FunnyOS::Bootloader32