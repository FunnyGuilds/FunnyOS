#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP

#include "Dynamic.hpp"
#include "DynamicString.hpp"
#include "Memory.hpp"
#include "System.hpp"
#include "Stream.hpp"
#include "String.hpp"

namespace FunnyOS::Stdlib {

    class File {
       public:
        TRIVIALLY_MOVEABLE(File);
        NON_COPYABLE(File);

        File(Stdlib::DynamicString name, Owner<IWriteInterface> writeInterface, Owner<IReadInterface> readInterface);

        const DynamicString& GetFileName() const;

        [[nodiscard]] bool IsReadOnly() const;

        [[nodiscard]] Owner<IWriteInterface>& GetWriteInterface();

        [[nodiscard]] Owner<IReadInterface>& GetReadInterface();

       private:
        DynamicString m_name;
        Owner<IWriteInterface> m_writeInterface;
        Owner<IReadInterface> m_readInterface;
    };

    enum FileOpenMode {
        FILE_OPEN_MODE_READ   = 0b00000001,
        FILE_OPEN_MODE_WRITE  = 0b00000010,
        FILE_OPEN_MODE_APPEND = 0b00000100
    };

    Optional<File> OpenFile(DynamicString fileName, FileOpenMode mode);

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP
