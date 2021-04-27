#include "Common.hpp"
#include <FunnyOS/Stdlib/HashMap.hpp>
#include <FunnyOS/Stdlib/DynamicString.hpp>
#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestHashmap, TestNewHashMap) {
    HashMap<int, DynamicString> map1{};
    HashMap<int, DynamicString> map2{3};
    HashMap<int, DynamicString> map3{5, 3};

    ASSERT_EQ(0, map1.Size()) << "newly created map is not empty";
    ASSERT_EQ(0, map2.Size()) << "newly created map is not empty";
    ASSERT_EQ(0, map3.Size()) << "newly created map is not empty";
}

TEST(TestHashmap, TestInsert) {
    HashMap<int, DynamicString> map{};

    map.Insert(0, DynamicString {"Hello world"});
    map.Insert(7, DynamicString {"This is test"});
    map.Insert(124, DynamicString {"some value"});
    map.Insert(140, DynamicString {"This is test"});

    ASSERT_EQ(4, map.Size()) << "map size is invalid";

    ASSERT_STREQ("Hello world", map[0].AsCString());
    ASSERT_STREQ("This is test", map[7].AsCString());
    ASSERT_STREQ("some value", map[124].AsCString());
    ASSERT_STREQ("This is test", map[140].AsCString());

    ASSERT_EQ(map.GetOptional(16), nullptr);
    ASSERT_EQ(map.GetOptional(12), nullptr);
    ASSERT_EQ(map.GetOptional(8), nullptr);
    ASSERT_EQ(map.GetOptional(23), nullptr);
}


