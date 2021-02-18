#include "Common.hpp"
#include <FunnyOS/Stdlib/DynamicString.hpp>

#include <gtest/gtest.h>

TEST(TestDynamicString, TestConstructor) {
    using namespace FunnyOS::Stdlib;

    DynamicString str1{"Hello, this is test"};
    DynamicString str2{"blah\0 blah blah"};
    DynamicString str3{"Hello, this is test", 7, 4};
    DynamicString str4{""};

    EXPECT_EQ(19, str1.Size());
    EXPECT_EQ(4, str2.Size());
    EXPECT_EQ(4, str3.Size());
    EXPECT_EQ(0, str4.Size());

    EXPECT_STREQ("Hello, this is test", str1.Begin());
    EXPECT_STREQ("blah", str2.Begin());
    EXPECT_STREQ("this", str3.Begin());
    EXPECT_STREQ("", str4.Begin());
}

TEST(TestDynamicString, TestAppend) {
    using namespace FunnyOS::Stdlib;

    DynamicString str1{"Hello, this "};
    DynamicString str2{"is "};
    DynamicString str3{"test"};

    DynamicString result{str1};
    result.Append(str2);
    result.Append(str3);

    EXPECT_EQ(str1.Size() + str2.Size() + str3.Size(), result.Size());
    EXPECT_STREQ("Hello, this is test", result.Begin());
}
