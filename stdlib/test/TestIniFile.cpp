#include "Common.hpp"
#include <FunnyOS/Stdlib/DynamicString.hpp>
#include <FunnyOS/Stdlib/IniFile.hpp>
#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

const char* testFile = R"INI_FILE(
key = value

[person]
name = John Doe

[database]

# Some comment
server=localhost
port=3306

)INI_FILE";

TEST(TestIniFile, TestReader) {
    IniFileReader reader{MakeOwnerBase<IReadInterface, FromMemoryReadInterface>(
        reinterpret_cast<const uint8_t*>(testFile), String::Length(testFile))};

    IniFile file = reader.Read();

    EXPECT_EQ(3, file.GetSectionNames().Size()) << "Invalid sections count";
    EXPECT_EQ(1, file.GetDefaultSection().GetKeyNames().Size()) << "Invalid keys count in the default section";
    EXPECT_EQ(1, file.GetSection("person").GetKeyNames().Size()) << "Invalid keys count in the person section";
    EXPECT_EQ(2, file.GetSection("database").GetKeyNames().Size()) << "Invalid keys count in the database section";

    EXPECT_STREQ("value", file.GetValue("", "key").AsCString());
    EXPECT_STREQ("John Doe", file.GetValue("person", "name").AsCString());
    EXPECT_STREQ("localhost", file.GetValue("database", "server").AsCString());
    EXPECT_STREQ("3306", file.GetValue("database", "port").AsCString());

    return;
}
