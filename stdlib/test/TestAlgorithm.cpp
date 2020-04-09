#include "Common.hpp"
#include <FunnyOS/Stdlib/Algorithm.hpp>

#include <gtest/gtest.h>

TEST(TestAlgorithm, TestMinMax)
{
    using namespace FunnyOS::Stdlib;

    EXPECT_EQ(Min(1, 2), 1);
    EXPECT_EQ(Min(2, 1), 1);

    EXPECT_EQ(Min(1.0F, 2.0F), 1.0F);
    EXPECT_EQ(Min(2.0F, 1.0F), 1.0F);

    EXPECT_EQ(Min(23, 23), 23);
    EXPECT_EQ(Min(23, 23), 23);

    EXPECT_EQ(Max(1, 2), 2);
    EXPECT_EQ(Max(2, 1), 2);

    EXPECT_EQ(Max(1.0F, 2.0F), 2.0F);
    EXPECT_EQ(Max(2.0F, 1.0F), 2.0F);

    EXPECT_EQ(Max(23, 23), 23);
    EXPECT_EQ(Max(23, 23), 23);
}