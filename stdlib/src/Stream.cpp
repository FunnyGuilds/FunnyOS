#include <FunnyOS/Stdlib/Stream.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>

namespace FunnyOS::Stdlib {

    WriteMode IWriteInterface::GetWriteMode() const {
        return m_writeMode;
    }

    void IWriteInterface::SetWriteMode(WriteMode writeMode) {
        m_writeMode = writeMode;
    }

    size_t IWriteInterface::GetCurrentOffset() const {
        return m_currentOffset;
    }

    void IWriteInterface::SetCurrentOffset(size_t currentOffset) {
        m_currentOffset = currentOffset;
    }

    const size_t& IReadInterface::GetCurrentOffset() const {
        return m_currentOffset;
    }

    void IReadInterface::SetCurrentOffset(const size_t& currentOffset) {
        m_currentOffset = currentOffset;
    }

    FromMemoryReadInterface::FromMemoryReadInterface(const uint8_t* data, const size_t size)
        : m_data{data}, m_size{size}, m_currentOffset{0} {}

    size_t FromMemoryReadInterface::ReadData(Memory::SizedBuffer<uint8_t>& buffer) {
        const size_t readSize = Min(buffer.Size, m_size - m_currentOffset);
        Memory::Copy(buffer.Data, m_data + m_currentOffset, readSize);
        m_currentOffset += readSize;
        return readSize;
    }

    size_t FromMemoryReadInterface::GetEstimatedSize() {
        return m_size;
    }

    void FromMemoryReadInterface::Close() {}

    Reader::Reader(Stdlib::Owner<IReadInterface> readInterface) : Reader{Move(readInterface), DEFAULT_CHUNK_SIZE} {}

    Reader::Reader(Stdlib::Owner<IReadInterface> readInterface, size_t chunkSize) : m_interface{Move(readInterface)} {
        m_buffer.Data = nullptr;
        Memory::ReallocateBuffer(m_buffer, chunkSize);
    }

    Optional<uint8_t> Reader::NextByte() {
        if (m_index >= m_currentSize) {
            DoRead();
        }

        if (m_currentSize == 0) {
            return EmptyOptional<uint8_t>();
        }

        return m_buffer.Data[m_index++];
    }

    size_t Reader::NextBytes(Memory::SizedBuffer<uint8_t>& buffer) {
        size_t left  = buffer.Size;
        size_t total = 0;

        while (left > 0) {
            if (m_index >= m_currentSize) {
                DoRead();

                if (m_currentSize == 0) {
                    return total;
                }
            }

            const size_t copySize = Min(m_currentSize - m_index, left);
            Memory::Copy(buffer.Data + total, m_buffer.Data + m_index, copySize);
            left -= copySize;
        }

        return total;
    }

    Vector<uint8_t> Reader::ReadWhole() {
        Vector<uint8_t> data;

        do {
            if (m_index < m_currentSize) {
                data.Insert(data.Size(), m_buffer.Data + m_index, m_currentSize - m_index);
            }

            DoRead();
        } while (m_currentSize != 0);

        return data;
    }

    void Reader::DoRead() {
        m_currentSize = m_interface->ReadData(m_buffer);
        m_index       = 0;
    }
}  // namespace FunnyOS::Stdlib