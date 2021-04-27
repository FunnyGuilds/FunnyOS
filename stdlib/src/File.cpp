#include <FunnyOS/Stdlib/File.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>
#include <FunnyOS/Stdlib/Platform.hpp>

namespace FunnyOS::Stdlib {
    File::File(DynamicString name, Owner<IWriteInterface> writeInterface, Owner<IReadInterface> readInterface)
        : m_name{Move(name)}, m_writeInterface(Move(writeInterface)), m_readInterface(Move(readInterface)) {}

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

    Optional<File> OpenFile(DynamicString fileName, FileOpenMode mode) {
        return _Platform::OpenFile(Move(fileName), mode);
    }

}  // namespace FunnyOS::Stdlib
