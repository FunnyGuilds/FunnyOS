#include <FunnyOS/Stdlib/File.hpp>

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

    File::File(DynamicString name, Owner<IWriteInterface> writeInterface, Owner<IReadInterface> readInterface)
        : m_name { Move(name) }, m_writeInterface(Move(writeInterface)), m_readInterface(Move(readInterface)) {

    }

    const DynamicString& File::GetFileName() const {
        return m_name;
    }

    bool File::IsReadOnly() const {
        return m_writeInterface.Get() != nullptr;
    }

    Owner<IWriteInterface>& File::GetWriteInterface() {
        return m_writeInterface;
    }

    Owner<IReadInterface>& File::GetReadInterface() {
        return m_readInterface;
    }

} // FunnyOS::Stdlib::File

