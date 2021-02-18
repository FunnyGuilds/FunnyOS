#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP

#include "Dynamic.hpp"
#include "DynamicString.hpp"
#include "Memory.hpp"
#include "System.hpp"
#include "String.hpp"

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

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_FILE_HPP
