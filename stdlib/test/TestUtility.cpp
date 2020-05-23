#include "Common.hpp"
#include "TrackableObject.hpp"
#include <FunnyOS/Stdlib/Functional.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestUtility, TestStorageEmpty) {
    TrackableObject::ResetAll();

    Storage<TrackableObject> test{};

    ASSERT_EQ(TrackableObject::GetCopyConstructionCount(), 0)
        << "Empty storage initialization called the object constructor";
}

TEST(TestUtility, TestStorageCopy) {
    {
        TrackableObject::ResetAll();
        bool flag = false;
        TrackableObject testObject{flag};
        flag = false;

        Storage<TrackableObject> test{InPlaceConstructorTag::Value, testObject};

        ASSERT_FALSE(flag) << "Copy storage initialization called the object constructor";
        ASSERT_EQ(TrackableObject::GetStandardConstructionCount(), 1)
            << "Copy storage initialization called the object constructor";
        ASSERT_EQ(TrackableObject::GetCopyConstructionCount(), 1)
            << "Copy storage initialization did not call the copy constructor";

        ASSERT_TRUE(test.GetObject().IsValidObject()) << "Copied object is invalid";
    }

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestUtility, TestStorageInPlace) {
    {
        bool flag = false;
        TrackableObject::ResetAll();

        Storage<TrackableObject> test{InPlaceConstructorTag::Value, flag};

        ASSERT_TRUE(flag) << "In-place storage initialization did not call the object constructor";
        ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), 1)
            << "In-place storage initialization did not call the object constructor";

        ASSERT_TRUE(test.GetObject().IsValidObject()) << "Constructed object is invalid";
    }

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestUtility, TestAssignments) {
    {
        TrackableObject::ResetAll();
        bool unused = false;
        TrackableObject test(unused);  // construction 1

        Storage<TrackableObject> inPlace{InPlaceConstructorTag::Value, unused};  // construction 2
        Storage<TrackableObject> copied{inPlace};                                // copy 1
        Storage<TrackableObject> moved{Move(inPlace)};                           // move 1

        ASSERT_EQ(TrackableObject::GetStandardConstructionCount(), 2) << "Construction count invalid";
        ASSERT_EQ(TrackableObject::GetMoveConstructionCount(), 1) << "Move count invalid";
        ASSERT_EQ(TrackableObject::GetCopyConstructionCount(), 1) << "Copy count invalid";
    }

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}