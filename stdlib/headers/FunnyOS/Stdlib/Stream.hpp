#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STREAM_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STREAM_HPP

#include "Dynamic.hpp"
#include "Memory.hpp"
#include "Vector.hpp"

namespace FunnyOS::Stdlib {

    enum class WriteMode { Replace, Append };

    class IWriteInterface {
        INTERFACE(IWriteInterface);

       public:
        virtual size_t WriteData(const Memory::SizedBuffer<uint8_t>& buffer) = 0;

        virtual void Flush() = 0;

        virtual void Close() = 0;

        [[nodiscard]] WriteMode GetWriteMode() const;

        void SetWriteMode(WriteMode writeMode);

        [[nodiscard]] size_t GetCurrentOffset() const;

        void SetCurrentOffset(size_t currentOffset);

       private:
        WriteMode m_writeMode;
        size_t m_currentOffset;
    };

    class IReadInterface {
        INTERFACE(IReadInterface);

       public:
        virtual size_t ReadData(Memory::SizedBuffer<uint8_t>& buffer) = 0;

        virtual size_t GetEstimatedSize() = 0;

        virtual void Close() = 0;

        [[nodiscard]] const size_t& GetCurrentOffset() const;

        void SetCurrentOffset(const size_t& currentOffset);

       private:
        size_t m_currentOffset{0};
    };

    class FromMemoryReadInterface : public IReadInterface {
       public:
        FromMemoryReadInterface(const uint8_t* data, const size_t size);

        ~FromMemoryReadInterface() override = default;

        size_t ReadData(Memory::SizedBuffer<uint8_t>& buffer) override;

        size_t GetEstimatedSize() override;

        void Close() override;

       private:
        const uint8_t* m_data;
        const size_t m_size;
        size_t m_currentOffset;
    };

    class Reader {
       public:
        constexpr static const size_t DEFAULT_CHUNK_SIZE = 512;

       public:
        Reader(Stdlib::Owner<IReadInterface> readInterface);

        Reader(Stdlib::Owner<IReadInterface> readInterface, size_t chunkSize);

        Optional<uint8_t> NextByte();

        size_t NextBytes(Memory::SizedBuffer<uint8_t>& buffer);

        Vector<uint8_t> ReadWhole();

       private:
        void DoRead();

       private:
        Memory::SizedBuffer<uint8_t> m_buffer{};
        size_t m_index{0};
        size_t m_currentSize{0};
        Stdlib::Owner<IReadInterface> m_interface;
    };

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_STREAM_HPP
