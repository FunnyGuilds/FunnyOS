#include "Common.hpp"
#include <FunnyOS/Stdlib/DynamicString.hpp>

#include <gtest/gtest.h>

TEST(TestDynamicString, TestConstructor) {
    using namespace FunnyOS::Stdlib;

    DynamicString str1{"Hello, this is test"};
    DynamicString str2{"blah\0 blah blah"};
    DynamicString str3{"Hello, this is test", 7, 4};
    DynamicString str4{""};
    DynamicString str5{11, "test"};

    EXPECT_EQ(19, str1.Size());
    EXPECT_EQ(4, str2.Size());
    EXPECT_EQ(4, str3.Size());
    EXPECT_EQ(0, str4.Size());
    EXPECT_EQ(11, str5.Size());

    EXPECT_STREQ("Hello, this is test", str1.Begin());
    EXPECT_STREQ("blah", str2.Begin());
    EXPECT_STREQ("this", str3.Begin());
    EXPECT_STREQ("", str4.Begin());
    EXPECT_STREQ("testtesttes", str5.Begin());
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


TEST(TestDynamicString, TestReplaceString) {
    using namespace FunnyOS::Stdlib;

    DynamicString str{"the test that will test the replacement"};
    str.Replace("test", "check");
    str.Replace("the", "a");

    EXPECT_STREQ("a check that will check a replacement", str.Begin());
}

TEST(TestDynamicString, TestReplaceCharacter) {
    using namespace FunnyOS::Stdlib;

    DynamicString str1{"Hello, this is test"};
    str1.Replace('t', 'a');

    EXPECT_STREQ("Hello, ahis is aesa", str1.Begin());
}

TEST(TestDynamicString, TestTrim) {
    using namespace FunnyOS::Stdlib;

    DynamicString str1 {"string"};
    DynamicString str2 {" very test  "};
    DynamicString str3 {"string   h \n "};

    str1.Trim();
    str2.Trim();
    str3.Trim();

    EXPECT_STREQ("string", str1.Begin());
    EXPECT_STREQ("very test", str2.Begin());
    EXPECT_STREQ("string   h", str3.Begin());
}

TEST(TestDynamicString, TestToLowercase) {
    using namespace FunnyOS::Stdlib;

    DynamicString str ("Hello, this is a tEst STRING to be converted to LOWER CASE");
    str.ToLowercase();

    EXPECT_STREQ("hello, this is a test string to be converted to lower case", str.Begin());
}

TEST(TestDynamicString, TestToUppercase) {
    using namespace FunnyOS::Stdlib;

    DynamicString str ("Hello, this is a tEst STRING to be converted to uPpER CASE");
    str.ToUppercase();
    EXPECT_STREQ("HELLO, THIS IS A TEST STRING TO BE CONVERTED TO UPPER CASE", str.Begin());
}

