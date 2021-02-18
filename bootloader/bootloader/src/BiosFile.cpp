#include "BiosFile.hpp"

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/QuickFat/QuickFat.h>
#include <FunnyOS/Hardware/BIOS.hpp>
#include "HighMemory.hpp"

namespace FunnyOS::Bootloader64 {

    class HighMemoryReadInterface : public Stdlib::IReadInterface {
       public:
        HighMemoryReadInterface(size_t size) {
            m_buffer.Data = static_cast<uint8_t*>(GetHighMemoryAllocator().Allocate(size, 1));
            m_buffer.Size = size;

            if (m_buffer.Data == nullptr) {
                F_ERROR_WITH_MESSAGE(FileSystemException, "Cannot allocate high memory buffer for file");
            }
        }

        ~HighMemoryReadInterface() {
            Close();
        }

        size_t ReadData(Stdlib::Memory::SizedBuffer<uint8_t>& buffer) override {
            Stdlib::Memory::Copy(buffer, m_buffer.Data + GetCurrentOffset());

            return buffer.Size;
        }

        size_t GetEstimatedSize() override {
            return m_buffer.Size;
        }

        void Close() override {
            if (m_buffer.Data != nullptr) {
                GetHighMemoryAllocator().Free(m_buffer.Data);
                m_buffer.Data = nullptr;
            }
        }

        Stdlib::Memory::SizedBuffer<uint8_t>& GetBuffer() {
            return m_buffer;
        }

       private:
        Stdlib::Memory::SizedBuffer<uint8_t> m_buffer;
    };

    namespace {
        QuickFat_Context& GetQFContext() {
            static QuickFat_Context c_context;
            return c_context;
        }

        void ThrowOnQFError(int error) {
            if (error == 0) {
                return;
            }

            Stdlib::String::StringBuffer buffer = Stdlib::String::AllocateBuffer(24);
            Stdlib::String::Format(buffer, "QF Error 0x%llx", error);

            F_ERROR_WITH_MESSAGE(FileSystemException, buffer.Data);
        }

        void ThrowOnBIOSError(int error) {
            if (error == 0) {
                return;
            }

            Stdlib::String::StringBuffer buffer = Stdlib::String::AllocateBuffer(24);
            Stdlib::String::Format(buffer, "BIOS error, ah = 0x%llx", error);

            F_ERROR_WITH_MESSAGE(FileSystemException, buffer.Data);
        }

        struct EDDDriveParameters {
            uint16_t BufferSize;
            uint16_t InformationFlags;
            uint32_t CylindersCount;
            uint32_t HeadsCount;
            uint32_t SectorsCount;
            uint64_t TotalSectorsCount;
            uint16_t BytesPerSector;
        } F_DONT_ALIGN;

        struct EDDPacket {
            uint8_t PacketSize;
            uint8_t Reserved;
            uint16_t NumberOfBlocks;
            uint16_t DestinationOffset;
            uint16_t DestinationSegment;
            uint64_t StartingLBA;
        } F_DONT_ALIGN;

        EDDDriveParameters& GetBootDriveParameters() {
            static EDDDriveParameters c_bootDriveParameters;
            return c_bootDriveParameters;
        }

        int BiosFileRead(void* data, uint32_t lba, uint32_t count, uint8_t* out) {
            const uint8_t drive = static_cast<uint8_t>(reinterpret_cast<uint64_t>(data));

            while (count > 0) {
                const uint16_t currentRead = Stdlib::Min<uint16_t>(count, 0x7F);

                EDDPacket packet{
                    .PacketSize         = sizeof(EDDPacket),
                    .Reserved           = 0,
                    .NumberOfBlocks     = currentRead,
                    .DestinationOffset  = HW::BIOS::GetRealModeOffset(out),
                    .DestinationSegment = HW::BIOS::GetRealModeSegment(out),
                    .StartingLBA        = lba};

                uint8_t error;
                if (!HW::BIOS::CallBios(0x13, "ah, dl, ds:si, =ah", 0x42, drive, &packet, &error)) {
                    return error == 0 ? 0xFF : error;
                }

                lba += currentRead;
                count -= currentRead;
                out += currentRead * GetBootDriveParameters().BytesPerSector;
            }

            return 0;
        }
    }  // namespace

    void InitializeFileSystem(Bootparams::BootDriveInfo& drive) {
        // Read drive info
        const uint8_t driveNumber               = drive.BootDriveNumber;
        EDDDriveParameters& bootDriveParameters = GetBootDriveParameters();

        Stdlib::Memory::ZeroMemory(bootDriveParameters);
        bootDriveParameters.BufferSize = sizeof(EDDDriveParameters);

        uint8_t error;
        if (!HW::BIOS::CallBios(0x13, "ah, dl, ds:si, =ah", 0x48, driveNumber, &bootDriveParameters, &error)) {
            ThrowOnBIOSError(error);
        }

        // Init QuickFat
        QuickFat_initialization_data initializationData;

        initializationData.partition_entry    = drive.BootPartition;
        initializationData.read_function      = &BiosFileRead;
        initializationData.read_function_data = reinterpret_cast<void*>(driveNumber);

        ThrowOnQFError(quickfat_init_context(&GetQFContext(), &initializationData));
    }

    Stdlib::Optional<Stdlib::File> OpenFile(const char* fileName) {
        QuickFat_File file;

        // open file
        uint32_t openError = quickfat_open_file(&GetQFContext(), &file, fileName);

        if (openError == QUICKFAT_ERROR_FAILED_TO_FIND_FILE) {
            return {};
        }

        ThrowOnQFError(openError);

        // allocate buffer and read
        auto readInterface = Stdlib::MakeOwner<HighMemoryReadInterface>(file.size);

        ThrowOnQFError(quickfat_read_file(&GetQFContext(), &file, readInterface->GetBuffer().Data));

        // create interfaces
        Stdlib::Owner<Stdlib::IWriteInterface> writeInterface{};

        return Stdlib::MakeOptional<Stdlib::File>(
            Stdlib::DynamicString{fileName}, Stdlib::Move(writeInterface),
            Stdlib::StaticOwnerCast<Stdlib::IReadInterface>(Stdlib::Move(readInterface)));
    }

}  // namespace FunnyOS::Bootloader64
