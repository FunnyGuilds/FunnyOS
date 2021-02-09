#include "Common.hpp"
#include <FunnyOS/Stdlib/String.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;
using namespace FunnyOS::Stdlib::String;

TEST(TestString, TestLength) {
    EXPECT_EQ(0, Length(""));
    EXPECT_EQ(0, Length("\0"));
    EXPECT_EQ(0, Length("\0abcdef"));

    EXPECT_EQ(1, Length("A"));
    EXPECT_EQ(1, Length("\x01"));
    EXPECT_EQ(1, Length("h\0ello"));

    EXPECT_EQ(21, Length("this is sample string"));
    EXPECT_EQ(21, Length("this is sAMple string"));
    EXPECT_EQ(21, Length("this is sample string\0"));
    EXPECT_EQ(20, Length("this is sample strin\0g"));
}

TEST(TestString, TestConcat) {
    StringBuffer buffer = Memory::AllocateBuffer<char>(128);

    Concat(buffer, "Hello, ", "world!");
    EXPECT_STREQ(buffer.Data, "Hello, world!");

    Concat(buffer, "Hello, \0world!", "world!\0world!");
    EXPECT_STREQ(buffer.Data, "Hello, world!");

    Concat(buffer, "Hello, ", "world!\0world!");
    EXPECT_STREQ(buffer.Data, "Hello, world!");

    Concat(buffer, "Hello, \0world!", "world!");
    EXPECT_STREQ(buffer.Data, "Hello, world!");

    Concat(buffer, "Hello, world!", "");
    EXPECT_STREQ(buffer.Data, "Hello, world!");

    Concat(buffer, "", "Hello, world!");
    EXPECT_STREQ(buffer.Data, "Hello, world!");
}

TEST(TestString, TestCompare) {
    EXPECT_GT(CompareWithMax("Hello", "Hallo", 10), 0);
    EXPECT_LT(CompareWithMax("Abc", "Adb", 10), 0);
    EXPECT_EQ(CompareWithMax("ABc", "ABc", 10), 0);

    EXPECT_EQ(CompareWithMax("This is a string", "This is a completely different string", 10), 0);
    EXPECT_GT(CompareWithMax("This is a string", "This is a completely different string", 11), 0);
}

TEST(TestString, TextNextToken) {
    auto buf = AllocateCopy("Hello, this is, a test string");

    char* data = buf.Data;

    EXPECT_STREQ("Hello", NextToken(&data, ","));
    EXPECT_STREQ(" this is", NextToken(&data, ","));
    EXPECT_STREQ(" a test string", NextToken(&data, ","));

    EXPECT_EQ(data, nullptr) << "Data not set to nullptr after all tokens depleted";
}

TEST(TestString, TestTrim) {
    auto buf1 = AllocateCopy("string");
    auto buf2 = AllocateCopy(" very test  ");
    auto buf3 = AllocateCopy("string   h \n ");

    char* data1 = buf1.Data;
    char* data2 = buf2.Data;
    char* data3 = buf3.Data;

    Trim(&data1);
    Trim(&data2);
    Trim(&data3);

    EXPECT_STREQ("string", data1);
    EXPECT_STREQ("very test", data2);
    EXPECT_STREQ("string   h", data3);
}

TEST(TestString, TestToLowercase) {
    auto buffer = AllocateCopy("Hello, this is a tEst STRING to be converted to LOWER CASE");
    ToLowercase(buffer.Data);

    EXPECT_STREQ("hello, this is a test string to be converted to lower case", buffer.Data);
}

TEST(TestString, TestToUppercase) {
    auto buffer = AllocateCopy("Hello, this is a tEst STRING to be converted to uPpER CASE");
    ToUppercase(buffer.Data);

    EXPECT_STREQ("HELLO, THIS IS A TEST STRING TO BE CONVERTED TO UPPER CASE", buffer.Data);
}

TEST(TestString, TestIntToString) {
    StringBuffer buffer = Memory::AllocateBuffer<char>(128);

    IntegerToString(buffer, 12);
    EXPECT_STREQ("12", buffer.Data);

    IntegerToString(buffer, -123);
    EXPECT_STREQ("-123", buffer.Data);

    IntegerToString<::uint8_t>(buffer, 255);
    EXPECT_STREQ("255", buffer.Data);

    IntegerToString<::int8_t>(buffer, -128);
    EXPECT_STREQ("-128", buffer.Data);

    IntegerToString(buffer, 0x1234, 16);
    EXPECT_STREQ("1234", buffer.Data);

    IntegerToString(buffer, 0b11001010, 2);
    EXPECT_STREQ("11001010", buffer.Data);

    IntegerToString(buffer, 0123321, 8);
    EXPECT_STREQ("123321", buffer.Data);
}

TEST(TestString, TestIntToHex) {
    StringBuffer buffer = Memory::AllocateBuffer<char>(128);

    IntegerToHex<uint16_t>(buffer, 0x1234);
    EXPECT_STREQ("1234", buffer.Data);

    IntegerToHex<uint32_t>(buffer, 0x1234);
    EXPECT_STREQ("00001234", buffer.Data);

    IntegerToHex<uint8_t>(buffer, 0xFF);
    EXPECT_STREQ("FF", buffer.Data);

    IntegerToHex<uint32_t>(buffer, 0);
    EXPECT_STREQ("00000000", buffer.Data);
}

TEST(TestString, TestStringToInt) {
    EXPECT_EQ(0, StringToInt<int>("0", 10).GetValue());
    EXPECT_EQ(5435, StringToInt<int>("5435", 10).GetValue());
    EXPECT_EQ(1234, StringToInt<int>("+1234", 10).GetValue());
    EXPECT_EQ(-99999, StringToInt<int>("-99999", 10).GetValue());
    EXPECT_EQ(0xEF12C0, StringToInt<int>("EF12C0", 16).GetValue());
    EXPECT_EQ(054323, StringToInt<int>("54323", 8).GetValue());

    EXPECT_FALSE(StringToInt<int>("", 10));
    EXPECT_FALSE(StringToInt<int>("no", 10));
    EXPECT_FALSE(StringToInt<int>("+1+234", 10));
    EXPECT_FALSE(StringToInt<int>("-99-999", 10));
    EXPECT_FALSE(StringToInt<int>("EF12C0G", 16));
    EXPECT_FALSE(StringToInt<int>("54323no", 8));

    EXPECT_FALSE(StringToIntLax<int>("", 10));
    EXPECT_FALSE(StringToIntLax<int>("no", 10));

    EXPECT_EQ(1, StringToIntLax<int>("+1+234", 10).GetValue());
    EXPECT_EQ(-99, StringToIntLax<int>("-99-999", 10).GetValue());
    EXPECT_EQ(0xEF12C0, StringToIntLax<int>("EF12C0G", 16).GetValue());
    EXPECT_EQ(054323, StringToIntLax<int>("54323no", 8).GetValue());
}

TEST(TestString, TestFormat) {
    StringBuffer buf = Memory::AllocateBuffer<char>(256);

    EXPECT_TRUE(String::Format(buf, "Hello, this is test"));
    EXPECT_STREQ(buf.Data, "Hello, this is test");

    EXPECT_TRUE(String::Format(buf, "Hello, %s", "this is test"));
    EXPECT_STREQ(buf.Data, "Hello, this is test");

    EXPECT_TRUE(String::Format(buf, "Hello, %d %x %X %o", 12, 0xFE, 0xEF, 01234));
    EXPECT_STREQ(buf.Data, "Hello, 12 fe EF 1234");

    EXPECT_TRUE(String::Format(buf, "Hello%%, %20d;%0-20d", -12, -12));
    EXPECT_STREQ(buf.Data, "Hello%, -12                 ;-0000000000000000012");

    EXPECT_TRUE(String::Format(buf, "Prefixed: %#010X;%#10X", 0xABCDEF, 0xABCDEF));
    EXPECT_STREQ(buf.Data, "Prefixed: 0X00ABCDEF;0XABCDEF  ");

    EXPECT_TRUE(String::Format(buf, "Signs: %d,%d,%+d,%+d,% d,% d", -12, 12, -12, 12, -12, 12));
    EXPECT_STREQ(buf.Data, "Signs: -12,12,-12,+12,-12, 12");
}