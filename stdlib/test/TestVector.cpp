#include "Common.hpp"
#include "TrackableObject.hpp"
#include <FunnyOS/Stdlib/Vector.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

TEST(TestVector, TestVectorCreation) {
    Vector<int> vector = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, vector.Size());
    EXPECT_EQ(6, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(3, vector[2]);
    EXPECT_EQ(4, vector[3]);
    EXPECT_EQ(5, vector[4]);
    EXPECT_EQ(6, vector[5]);
}

TEST(TestVector, TestVectorAppend) {
    Vector<int> vector(0, 1.5f);
    vector.Append(2);
    vector.Append(6);

    EXPECT_EQ(2, vector.Size());
    EXPECT_EQ(2, vector.Capacity());
    EXPECT_EQ(2, vector[0]);
    EXPECT_EQ(6, vector[1]);

    vector.Append(8);
    vector.Append(21);
    vector.Append(77);

    EXPECT_EQ(5, vector.Size());
    EXPECT_EQ(2, vector[0]);
    EXPECT_EQ(6, vector[1]);
    EXPECT_EQ(8, vector[2]);
    EXPECT_EQ(21, vector[3]);
    EXPECT_EQ(77, vector[4]);

    try {
        static_cast<void>(vector[5]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestVector, CheckAllocations) {
    {
        TrackableObject::ResetAll();

        bool flag = false;

        Vector<TrackableObject> vector = {TrackableObject{flag}, TrackableObject{flag}, TrackableObject{flag}};
        EXPECT_EQ(3, vector.Size());

        for (auto& ref : vector) {
            EXPECT_TRUE(ref.IsValidObject()) << "Object is invalid";
        }
    }

    EXPECT_EQ(TrackableObject::GetStandardConstructionCount(), 3);
    EXPECT_EQ(TrackableObject::GetCopyConstructionCount(), 3);

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestVector, CheckRemove) {
    Vector<int> vector = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, vector.Size());
    EXPECT_GE(6, vector.Capacity());

    vector.Remove(3);
    EXPECT_EQ(5, vector.Size());
    EXPECT_GE(6, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(3, vector[2]);
    EXPECT_EQ(5, vector[3]);
    EXPECT_EQ(6, vector[4]);

    try {
        static_cast<void>(vector[5]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }

    vector.ShrinkToSize();
    EXPECT_EQ(5, vector.Size());
    EXPECT_GE(5, vector.Capacity());
}

TEST(TestVector, CheckRemoveLast) {
    Vector<int> vector = {1, 2, 3, 4, 5, 6};

    EXPECT_EQ(6, vector.Size());
    EXPECT_GE(6, vector.Capacity());

    vector.Remove(5);
    EXPECT_EQ(5, vector.Size());
    EXPECT_GE(6, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(3, vector[2]);
    EXPECT_EQ(4, vector[3]);
    EXPECT_EQ(5, vector[4]);

    try {
        static_cast<void>(vector[5]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }

    vector.ShrinkToSize();
    EXPECT_EQ(5, vector.Size());
    EXPECT_GE(5, vector.Capacity());
}

TEST(TestVector, CheckRemoveRange) {
    Vector<int> vector = {1, 2, 3, 4, 5, 6, 7, 8};

    EXPECT_EQ(8, vector.Size());
    EXPECT_LE(8, vector.Capacity());

    vector.RemoveRange(2, 4);
    EXPECT_EQ(5, vector.Size());
    EXPECT_LE(8, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(6, vector[2]);
    EXPECT_EQ(7, vector[3]);
    EXPECT_EQ(8, vector[4]);

    EXPECT_EQ(5, vector.Size());
    EXPECT_LE(8, vector.Capacity());

    try {
        static_cast<void>(vector[5]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }

    vector.ShrinkToSize();
    EXPECT_EQ(5, vector.Size());
    EXPECT_GE(5, vector.Capacity());
}

TEST(TestVector, CheckErase) {
    Vector<int> vector = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (auto it = Begin(vector); it != vector.End();) {
        if (*it >= 3 && *it <= 5) {
            it = vector.Erase(it);
        } else {
            it++;
        }
    }

    for (auto it = Begin(vector); it != vector.End();) {
        if (*it >= 9) {
            it = vector.Erase(it);
        } else {
            it++;
        }
    }

    EXPECT_EQ(5, vector.Size());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(6, vector[2]);
    EXPECT_EQ(7, vector[3]);
    EXPECT_EQ(8, vector[4]);

    try {
        static_cast<void>(vector[5]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestVector, CheckRemoveAllocations) {
    {
        TrackableObject::ResetAll();
        Vector<TrackableObject> vector = {
            TrackableObject{},
            TrackableObject{},
        };

        // Initializer list object
        EXPECT_EQ(TrackableObject::GetStandardConstructionCount(), 2);
        EXPECT_EQ(TrackableObject::GetDestructionCount(), 2);

        // Object in vectors check
        EXPECT_EQ(TrackableObject::GetCopyConstructionCount(), 2);

        // Remove element
        EXPECT_EQ(2, vector.Size());
        vector.Remove(0);
        EXPECT_EQ(1, vector.Size());
        EXPECT_EQ(TrackableObject::GetDestructionCount(), 3);

        // Force reallocation
        vector.ShrinkToSize();
    }

    ASSERT_EQ(TrackableObject::GetTotalConstructionCount(), TrackableObject::GetDestructionCount())
        << "Construction count != destruction count";
}

TEST(TestVector, CheckInsert) {
    Vector<int> vector = {1, 2, 3, 5, 6};

    EXPECT_EQ(5, vector.Size());
    EXPECT_LE(5, vector.Capacity());

    vector.Insert(3, 4);
    EXPECT_EQ(6, vector.Size());
    EXPECT_LE(6, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(3, vector[2]);
    EXPECT_EQ(4, vector[3]);
    EXPECT_EQ(5, vector[4]);
    EXPECT_EQ(6, vector[5]);

    try {
        static_cast<void>(vector[6]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }
}

TEST(TestVector, CheckInsertRange) {
    Vector<int> toInsert = {3, 4, 5};

    Vector<int> vector = {1, 2, 6, 7, 8};

    EXPECT_EQ(5, vector.Size());
    EXPECT_LE(5, vector.Capacity());

    vector.Insert(2, toInsert.Begin(), 3);
    EXPECT_EQ(8, vector.Size());
    EXPECT_LE(8, vector.Capacity());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
    EXPECT_EQ(3, vector[2]);
    EXPECT_EQ(4, vector[3]);
    EXPECT_EQ(5, vector[4]);
    EXPECT_EQ(6, vector[5]);
    EXPECT_EQ(7, vector[6]);
    EXPECT_EQ(8, vector[7]);

    try {
        static_cast<void>(vector[8]);
        FAIL() << "Exception not thrown";
    } catch (const VectorIndexOutOfBounds& ex) {
        // OK
    }
}
