#include <FunnyOS/Stdlib/IniFile.hpp>

namespace FunnyOS::Stdlib {
    DynamicString IniSection::GetValue(const DynamicString& key) const {
        const DynamicString* str = m_values.GetOptional(key);

        return str == nullptr ? "" : *str;
    }

    void IniSection::SetValue(const DynamicString& key, DynamicString value) {
        m_values.InsertOrReplace(key, Move(value));
    }

    Vector<DynamicString> IniSection::GetKeyNames() const {
        Vector<DynamicString> vector{};

        for (const auto& entry : m_values) {
            vector.AppendInPlace(entry.Key);
        }

        return vector;
    }

    DynamicString IniFile::GetValue(const DynamicString& sectionName, const DynamicString& key) const {
        const IniSection* section = m_sections.GetOptional(sectionName);
        if (section == nullptr) {
            return DynamicString{};
        }

        return section->GetValue(key);
    }

    DynamicString IniFile::GetValueOrDefault(
        const DynamicString& section, const DynamicString& key, const char* defaultValue) const {
        const DynamicString value = GetValue(section, key);

        if (!value) {
            return DynamicString{defaultValue};
        }

        return value;
    }

    void IniFile::SetValue(const DynamicString& sectionName, const DynamicString& key, DynamicString value) {
        GetSection(sectionName).SetValue(key, value);
    }

    IniSection& IniFile::GetDefaultSection() {
        return GetSection("");
    }

    const IniSection* IniFile::GetDefaultSection() const {
        return GetSection("");
    }

    IniSection& IniFile::GetSection(const DynamicString& name) {
        IniSection* section = m_sections.GetOptional(name);

        if (section == nullptr) {
            section = m_sections.Insert(name, IniSection{});
        }

        return *section;
    }

    const IniSection* IniFile::GetSection(const DynamicString& name) const {
        return m_sections.GetOptional(name);
    }

    Vector<DynamicString> IniFile::GetSectionNames() const {
        Vector<DynamicString> vector{};

        for (const auto& entry : m_sections) {
            vector.AppendInPlace(entry.Key);
        }

        return vector;
    }

    IniFileReader::IniFileReader(Owner<IReadInterface> readInterface) : m_reader(Stdlib::Move(readInterface)) {}

    enum class IniFileLocation { NEW_LINE, END_OF_LINE, COMMENT, SECTION, KEY, VALUE };

    IniFile IniFileReader::Read() {
        IniFile file;

        Optional<uint8_t> byte;
        IniFileLocation location = IniFileLocation::NEW_LINE;
        DynamicString currentSection{""};
        DynamicString currentKey;
        DynamicString currentValue;

        while ((byte = m_reader.NextByte())) {
            const char character = static_cast<char>(byte.GetValue());

            switch (location) {
                case IniFileLocation::NEW_LINE: {
                    if (String::IsWhitespace(character)) {
                        break;
                    }

                    if (character == '#') {
                        location = IniFileLocation::COMMENT;
                        break;
                    }

                    if (character == '=') {
                        F_ERROR_WITH_MESSAGE(IniFileInvalidException, "Line starts with =");
                    }

                    if (character == '[') {
                        location = IniFileLocation::SECTION;
                        currentSection.Clear();
                        break;
                    }

                    currentKey.Clear();
                    currentKey.Append(character);
                    location = IniFileLocation::KEY;
                    break;
                }
                case IniFileLocation::END_OF_LINE: {
                    if (character == '\n') {
                        location = IniFileLocation::NEW_LINE;
                        break;
                    }

                    if (String::IsWhitespace(character)) {
                        break;
                    }

                    F_ERROR_WITH_MESSAGE(IniFileInvalidException, "Expected new line");
                }
                case IniFileLocation::COMMENT: {
                    if (character == '\n') {
                        location = IniFileLocation::NEW_LINE;
                        break;
                    }

                    break;
                }
                case IniFileLocation::SECTION: {
                    if (character == '\n') {
                        F_ERROR_WITH_MESSAGE(IniFileInvalidException, "Section never closed");
                    }

                    if (character == ']') {
                        currentSection.Trim();
                        location = IniFileLocation::END_OF_LINE;
                        break;
                    }

                    currentSection.Append(character);
                    break;
                }
                case IniFileLocation::KEY: {
                    if (character == '=') {
                        currentKey.Trim();
                        currentValue.Clear();
                        location = IniFileLocation::VALUE;
                        break;
                    }

                    if (character == '\n') {
                        F_ERROR_WITH_MESSAGE(IniFileInvalidException, "Key with no value");
                    }

                    currentKey.Append(character);
                    break;
                }
                case IniFileLocation::VALUE: {
                    if (character == '\n') {
                        currentValue.Trim();
                        file.SetValue(currentSection, currentKey, currentValue);

                        location = IniFileLocation::NEW_LINE;
                        break;
                    }

                    currentValue.Append(character);
                    break;
                }
            }
        }

        if (location == IniFileLocation::VALUE) {
            currentValue.Trim();
            file.SetValue(currentSection, currentKey, currentValue);
        }

        return file;
    }

}  // namespace FunnyOS::Stdlib
