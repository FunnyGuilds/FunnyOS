#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-no-malloc"
#pragma ide diagnostic ignored "hicpp-no-malloc"

#include "Common.hpp"

#include <memory>
#include <fstream>
#include <cstring>
#include <malloc.h>
#include <FunnyOS/Stdlib/Platform.hpp>

namespace FunnyOS::_Platform {
    using namespace FunnyOS::Stdlib;

    void* AllocateMemoryAligned(size_t size, size_t aligned) noexcept {
        return aligned_alloc(aligned, size);
    }

    void* ReallocateMemoryAligned(void* memory, size_t size, size_t alignment) noexcept {
        void* newMem = AllocateMemoryAligned(size, alignment);
        if (newMem == nullptr) {
            return nullptr;
        }

        memcpy(newMem, memory, std::min(size, malloc_usable_size(memory)));
        free(memory);
        return newMem;
    }

    void FreeMemory(void* memory) noexcept {
        return free(memory);
    }

    void ReportError(const char* error) noexcept {
        fprintf(stderr, "%s", error);
    }

    void Terminate(const char* error) noexcept {
        ReportError(error);
        std::terminate();
    }

    class StdlibFileReadInterface : public Stdlib::IReadInterface {
       public:
        StdlibFileReadInterface(std::shared_ptr<std::fstream> stream) : m_stream{Move(stream)} {
            m_stream->seekg(0, m_stream->end);
            m_estimatedSize = m_stream->tellg();
            m_stream->seekg(0, m_stream->beg);
        }

        ~StdlibFileReadInterface() override = default;

        Stdlib::size_t ReadData(Memory::SizedBuffer<uint8_t>& buffer) override {
            m_stream->seekg(GetCurrentOffset(), m_stream->beg);
            m_stream->read(reinterpret_cast<char*>(buffer.Data), buffer.Size);
            SetCurrentOffset(GetCurrentOffset() + m_stream->gcount());
            return m_stream->gcount();
        }

        Stdlib::size_t GetEstimatedSize() override {
            return m_estimatedSize;
        }

        void Close() override {
            m_stream.reset();
        }

       private:
        std::shared_ptr<std::fstream> m_stream;
        Stdlib::size_t m_estimatedSize;
    };

    class StdlibFileWriteInterface : public Stdlib::IWriteInterface {
       public:
        StdlibFileWriteInterface(std::shared_ptr<std::fstream> stream) : m_stream{Move(stream)} {}

        ~StdlibFileWriteInterface() override = default;

        size_t WriteData(const Memory::SizedBuffer<uint8_t>& buffer) override {
            m_stream->write(reinterpret_cast<char*>(buffer.Data), buffer.Size);
            return buffer.Size;
        }

        void Flush() override {
            m_stream->flush();
        }

        void Close() override {
            Flush();
            m_stream.reset();
        }

       private:
        std::shared_ptr<std::fstream> m_stream;
    };

    Stdlib::Optional<Stdlib::File> OpenFile(Stdlib::DynamicString path, Stdlib::FileOpenMode mode) noexcept {
        Stdlib::Owner<IReadInterface> readInterface{};
        Stdlib::Owner<IWriteInterface> writeInterface{};

        std::ios::openmode openMode = std::ios::binary;

        if (mode & FILE_OPEN_MODE_READ) {
            openMode |= std::ios::in;
        }

        if (mode & FILE_OPEN_MODE_WRITE || mode & FILE_OPEN_MODE_APPEND) {
            openMode |= std::ios::out;
        }

        if (mode & FILE_OPEN_MODE_WRITE && !(mode & FILE_OPEN_MODE_APPEND)) {
            openMode |= std::ios::trunc;
        }

        auto stream = std::make_shared<std::fstream>(path.AsCString(), openMode);

        if (!stream->good()) {
            return EmptyOptional<Stdlib::File>();
        }

        if (openMode & std::ios::in) {
            readInterface.Reset(new StdlibFileReadInterface(stream));
        }

        if (openMode & std::ios::out) {
            writeInterface.Reset(new StdlibFileWriteInterface(stream));
        }

        return Stdlib::MakeOptional<Stdlib::File>(path, Move(writeInterface), Move(readInterface));
    }
}  // namespace FunnyOS::_Platform

#pragma clang diagnostic pop