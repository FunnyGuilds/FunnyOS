#include "Common.hpp"
#include <FunnyOS/Stdlib/File.hpp>

#include <gtest/gtest.h>

using namespace std;

TEST(TestFile, TestFileReadWrite) {
    using namespace FunnyOS::Stdlib;

    // write file
    String::StringBuffer stringToWrite = String::AllocateCopy("Hello, \r\nthis is file test\r\n");
    Memory::SizedBuffer<uint8_t> writeBuffer{reinterpret_cast<uint8_t*>(stringToWrite.Data), stringToWrite.Size - 1};

    File writeFile = Move(OpenFile("file_test.txt", FILE_OPEN_MODE_WRITE).GetValue());
    EXPECT_EQ(writeFile.GetWriteInterface()->WriteData(writeBuffer), writeBuffer.Size);
    writeFile.GetWriteInterface()->Close();

    // read file
    File readFile = Move(OpenFile("file_test.txt", FILE_OPEN_MODE_READ).GetValue());

    Reader reader{Move(readFile.GetReadInterface()), 2};
    Vector<uint8_t> readBuffer = reader.ReadWhole();
    readBuffer.Append(0);

    EXPECT_EQ(stringToWrite.Size, readBuffer.Size());
    EXPECT_STREQ(stringToWrite.Data, reinterpret_cast<char*>(readBuffer.Begin()));
}