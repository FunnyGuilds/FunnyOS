#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INIFILE_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INIFILE_HPP

#include "File.hpp"
#include "HashMap.hpp"

namespace FunnyOS::Stdlib {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(IniFileInvalidException);

    class IniSection {
       public:
        TRIVIALLY_MOVEABLE(IniSection);

        IniSection() = default;

        DynamicString GetValue(const DynamicString& key) const;

        void SetValue(const DynamicString& key, DynamicString value);

        Vector<DynamicString> GetKeyNames() const;

       private:
        HashMap<DynamicString, DynamicString> m_values;
    };

    class IniFile {
       public:
        DynamicString GetValue(const DynamicString& section, const DynamicString& key) const;

        DynamicString GetValueOrDefault(const DynamicString& section, const DynamicString& key, const char* defaultValue) const;

        void SetValue(const DynamicString& section, const DynamicString& key, DynamicString value);

        IniSection& GetDefaultSection();

        const IniSection* GetDefaultSection() const;

        IniSection& GetSection(const DynamicString& name);

        const IniSection* GetSection(const DynamicString& name) const;

        Vector<DynamicString> GetSectionNames() const;
       private:
        HashMap<DynamicString, IniSection> m_sections;
    };

    class IniFileReader {
       public:
        IniFileReader(Owner<IReadInterface> readInterface);

        IniFile Read();

       private:
        Reader m_reader;
    };

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_INIFILE_HPP
