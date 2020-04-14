#include "Common.hpp"
#include "TrackableObject.hpp"
#include <FunnyOS/Stdlib/LinkedList.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestLinkedList, TestLinkedListCreation) {
    LinkedList<int> list = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(4, list[3]);
    EXPECT_EQ(5, list[4]);
    EXPECT_EQ(6, list[5]);
}

TEST(TestLinkedList, TestLinkedListAppend) {
    LinkedList<int> list;
    list.Append(2);
    list.Append(6);

    EXPECT_EQ(2, list.Size());
    EXPECT_EQ(2, list[0]);
    EXPECT_EQ(6, list[1]);

    list.Append(8);
    list.Append(21);
    list.Append(77);

    EXPECT_EQ(5, list.Size());
    EXPECT_EQ(2, list[0]);
    EXPECT_EQ(6, list[1]);
    EXPECT_EQ(8, list[2]);
    EXPECT_EQ(21, list[3]);
    EXPECT_EQ(77, list[4]);

    try {
        static_cast<void>(list[5]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestLinkedList, CheckAllocations) {
    {
        TrackableObject::ResetAll();

        bool flag = false;

        LinkedList<TrackableObject> list = {TrackableObject{flag}, TrackableObject{flag}, TrackableObject{flag}};
        EXPECT_EQ(3, list.Size());

        for (auto& ref : list) {
            EXPECT_TRUE(ref.IsValidObject()) << "Object is invalid";
        }
    }

    EXPECT_EQ(TrackableObject::GetStandardConstructionCount(), 3);
    EXPECT_EQ(TrackableObject::GetCopyConstructionCount(), 3);

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestLinkedList, CheckRemove) {
    LinkedList<int> list = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, list.Size());

    list.Remove(3);
    EXPECT_EQ(5, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(5, list[3]);
    EXPECT_EQ(6, list[4]);

    try {
        static_cast<void>(list[5]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestLinkedList, CheckRemoveLast) {
    LinkedList<int> list = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, list.Size());

    list.Remove(5);
    EXPECT_EQ(5, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(4, list[3]);
    EXPECT_EQ(5, list[4]);

    try {
        static_cast<void>(list[5]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestLinkedList, CheckRemoveRange) {
    LinkedList<int> list = {1, 2, 3, 4, 5, 6, 7, 8};

    EXPECT_EQ(8, list.Size());

    list.RemoveRange(2, 4);
    EXPECT_EQ(5, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(6, list[2]);
    EXPECT_EQ(7, list[3]);
    EXPECT_EQ(8, list[4]);

    EXPECT_EQ(5, list.Size());

    try {
        static_cast<void>(list[5]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestLinkedList, CheckRemoveAllocations) {
    {
        TrackableObject::ResetAll();
        LinkedList<TrackableObject> list = {
            TrackableObject{},
            TrackableObject{},
        };

        // Initializer list object
        EXPECT_EQ(TrackableObject::GetStandardConstructionCount(), 2);
        EXPECT_EQ(TrackableObject::GetDestructionCount(), 2);

        // Object in lists check
        EXPECT_EQ(TrackableObject::GetCopyConstructionCount(), 2);

        // Remove element
        EXPECT_EQ(2, list.Size());
        list.Remove(0);
        EXPECT_EQ(1, list.Size());
        EXPECT_EQ(TrackableObject::GetDestructionCount(), 3);
    }

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestLinkedList, CheckInsert) {
    LinkedList<int> list = {1, 2, 3, 5, 6};

    EXPECT_EQ(5, list.Size());

    list.Insert(3, 4);
    EXPECT_EQ(6, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(4, list[3]);
    EXPECT_EQ(5, list[4]);
    EXPECT_EQ(6, list[5]);

    try {
        static_cast<void>(list[6]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}


TEST(TestLinkedList, CheckInsertRangeEnd) {
    LinkedList<int> toInsert = {6, 7, 8, 9};

    LinkedList<int> list = {1, 2, 3, 4, 5};

    EXPECT_EQ(5, list.Size());

    list.Insert(list.Size(), toInsert.Begin(), 4);
    EXPECT_EQ(9, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(4, list[3]);
    EXPECT_EQ(5, list[4]);
    EXPECT_EQ(6, list[5]);
    EXPECT_EQ(7, list[6]);
    EXPECT_EQ(8, list[7]);
    EXPECT_EQ(9, list[8]);

    try {
        static_cast<void>(list[9]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}


TEST(TestLinkedList, CheckInsertRange) {
    LinkedList<int> toInsert = {3, 4, 5};

    LinkedList<int> list = {1, 2, 6, 7, 8};

    EXPECT_EQ(5, list.Size());

    list.Insert(2, toInsert.Begin(), 3);
    EXPECT_EQ(8, list.Size());
    EXPECT_EQ(1, list[0]);
    EXPECT_EQ(2, list[1]);
    EXPECT_EQ(3, list[2]);
    EXPECT_EQ(4, list[3]);
    EXPECT_EQ(5, list[4]);
    EXPECT_EQ(6, list[5]);
    EXPECT_EQ(7, list[6]);
    EXPECT_EQ(8, list[7]);

    try {
        static_cast<void>(list[8]);
        FAIL() << "Exception not thrown";
    } catch (const LinkedListIndexOutOfBounds& ex) {
        // OK
    }
}
