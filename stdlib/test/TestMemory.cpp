#include "Common.hpp"
#include <FunnyOS/Stdlib/Memory.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestMemory, TestCopy) {
    Memory::SizedBuffer<uint8_t> destination = Memory::AllocateBuffer<uint8_t>(5);
    uint8_t source[] = {1, 2, 3, 4, 5};

    Memory::Copy(destination, source);

    EXPECT_EQ(1, destination.Data[0]);
    EXPECT_EQ(2, destination.Data[1]);
    EXPECT_EQ(3, destination.Data[2]);
    EXPECT_EQ(4, destination.Data[3]);
    EXPECT_EQ(5, destination.Data[4]);
}


TEST(TestMemory, TestSet) {
    Memory::SizedBuffer<uint8_t> destination = Memory::AllocateBuffer<uint8_t>(5);
    Memory::Set<uint8_t>(destination, 7);

    EXPECT_EQ(7, destination.Data[0]);
    EXPECT_EQ(7, destination.Data[1]);
    EXPECT_EQ(7, destination.Data[2]);
    EXPECT_EQ(7, destination.Data[3]);
    EXPECT_EQ(7, destination.Data[4]);
}


TEST(TestMemory, TestFillDivisible) {
    Memory::SizedBuffer<uint8_t> destination = Memory::AllocateBuffer<uint8_t>(8);
    Memory::SizedBuffer<uint8_t> pattern = Memory::AllocateBuffer<uint8_t>(2);

    pattern.Data[0] = 4;
    pattern.Data[1] = 22;

    Memory::Fill(destination, pattern);

    EXPECT_EQ(4, destination.Data[0]);
    EXPECT_EQ(22, destination.Data[1]);
    EXPECT_EQ(4, destination.Data[2]);
    EXPECT_EQ(22, destination.Data[3]);
    EXPECT_EQ(4, destination.Data[4]);
    EXPECT_EQ(22, destination.Data[5]);
    EXPECT_EQ(4, destination.Data[6]);
    EXPECT_EQ(22, destination.Data[7]);
}

TEST(TestMemory, TestFillNonDivisible) {
    Memory::SizedBuffer<uint8_t> destination = Memory::AllocateBuffer<uint8_t>(8);
    Memory::SizedBuffer<uint8_t> pattern = Memory::AllocateBuffer<uint8_t>(3);

    pattern.Data[0] = 4;
    pattern.Data[1] = 22;
    pattern.Data[2] = 121;

    Memory::Fill(destination, pattern);

    EXPECT_EQ(4, destination.Data[0]);
    EXPECT_EQ(22, destination.Data[1]);
    EXPECT_EQ(121, destination.Data[2]);
    EXPECT_EQ(4, destination.Data[3]);
    EXPECT_EQ(22, destination.Data[4]);
    EXPECT_EQ(121, destination.Data[5]);
    EXPECT_EQ(4, destination.Data[6]);
    EXPECT_EQ(22, destination.Data[7]);
}