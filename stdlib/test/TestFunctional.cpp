#include "Common.hpp"
#include <FunnyOS/Stdlib/Functional.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestFunctional, TestOptional) {
    try {
        Optional<int> empty1{};
        Optional<int> empty2 = EmptyOptional<int>();

        ASSERT_FALSE(empty1.HasValue()) << "empty optional has a value";
        ASSERT_FALSE(empty2.HasValue()) << "empty optional has a value";

        int value = 14;

        Optional<int> nonempty1{15};
        Optional<int> nonempty2{value};

        ASSERT_TRUE(nonempty1.HasValue()) << "non-empty optional does not have a value";
        ASSERT_TRUE(nonempty2.HasValue()) << "non-empty optional does not have a value";
        ASSERT_EQ(nonempty1.GetValue(), 15) << "Invalid optional value";
        ASSERT_EQ(nonempty2.GetValue(), value) << "Invalid optional value";

        Optional<int> empty3{};
        Optional<int> nonempty3{16};
        ASSERT_EQ(empty3.GetValueOrDefault(17), 17) << "Invalid optional value";
        ASSERT_EQ(nonempty3.GetValueOrDefault(17), 16) << "Invalid optional value";
    } catch (const System::Exception& e) {
        System::ReportError(e.GetMessage());
    }
}