#include "Common.hpp"
#include <FunnyOS/Stdlib/Algorithm.hpp>

#include <FunnyOS/Stdlib/LinkedList.hpp>
#include <FunnyOS/Stdlib/Vector.hpp>
#include <gtest/gtest.h>

TEST(TestAlgorithm, TestMinMax) {
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

    EXPECT_EQ(Min(1, 2, 3, 4), 1);
    EXPECT_EQ(Min(2, 1, 4, 3), 1);

    EXPECT_EQ(Max(1, 0, 2, 3, 8, 4), 8);
    EXPECT_EQ(Max(2, 7, 1, 4, 9, 3), 9);
}

namespace {
    template <typename Container>
    void DoTestRemoveIf(Container& container) {
        using namespace FunnyOS::Stdlib;

        RemoveIf(container, [](int it) { return it >= 3 && it <= 5; });
        RemoveIf(container, [](int it) { return it >= 9; });

        EXPECT_EQ(5, container.Size());
        EXPECT_EQ(1, container[0]);
        EXPECT_EQ(2, container[1]);
        EXPECT_EQ(6, container[2]);
        EXPECT_EQ(7, container[3]);
        EXPECT_EQ(8, container[4]);
    }
}  // namespace

TEST(TestAlgorithm, TestRemoveIf) {
    using namespace FunnyOS::Stdlib;

    Vector<int> vector = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    LinkedList<int> linkedList = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    DoTestRemoveIf(vector);
    DoTestRemoveIf(linkedList);
}