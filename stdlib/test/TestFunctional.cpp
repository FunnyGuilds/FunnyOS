#include "Common.hpp"
#include <FunnyOS/Stdlib/Functional.hpp>

#include <gtest/gtest.h>

using namespace FunnyOS::Stdlib;

namespace {
    constexpr const ::size_t validObjectFlag = 0x5634312;

    bool g_globalFlag = false;

    class StorageTest {
       public:
        explicit StorageTest(bool& flag) {
            flag = true;
            g_globalFlag = true;
            m_testFlag = validObjectFlag;

            std::cout << "Constructed " << static_cast<void*>(this) << "\n";
        }

        ~StorageTest() {
            std::cout << "Deconstructed " << static_cast<void*>(this) << "\n";
        }

        StorageTest(const StorageTest& other) {
            m_testFlag = other.m_testFlag;
            std::cout << "Copied " << static_cast<void*>(this) << "\n";
        }

        StorageTest& operator=(const StorageTest& other) {
            m_testFlag = other.m_testFlag;
            std::cout << "Copy assigned " << static_cast<void*>(this) << "\n";
            return *this;
        }

        StorageTest(StorageTest&& other) {
            m_testFlag = other.m_testFlag;
            other.m_testFlag = 0;
            std::cout << "Moved " << static_cast<void*>(this) << "\n";
        }

        StorageTest& operator=(StorageTest&& other) {
            m_testFlag = other.m_testFlag;
            other.m_testFlag = 0;
            std::cout << "Move assigned " << static_cast<void*>(this) << "\n";
            return *this;
        }

        ::size_t m_testFlag;
    };
}  // namespace

TEST(TestFunctional, TestStorageEmpty) {
    g_globalFlag = false;

    Storage<StorageTest> test{};

    ASSERT_FALSE(g_globalFlag) << "Empty storage initialization called the object constructor";
}

TEST(TestFunctional, TestStorageCopy) {
    bool flag = false;
    StorageTest testObject{flag};
    flag = false;
    g_globalFlag = false;

    Storage<StorageTest> test{InPlaceConstructor::Value, Move(testObject)};

    ASSERT_FALSE(flag) << "Copy storage initialization called the object constructor";
    ASSERT_FALSE(g_globalFlag) << "Copy storage initialization called the object constructor";
    ASSERT_EQ(test.GetObject().m_testFlag, validObjectFlag) << "Copied object is invalid";
}

TEST(TestFunctional, TestStorageInPlace) {
    bool flag = false;
    g_globalFlag = false;

    Storage<StorageTest> test{InPlaceConstructor::Value, flag};

    ASSERT_TRUE(flag) << "In-place storage initialization did not call the object constructor";
    ASSERT_TRUE(g_globalFlag) << "In-place storage initialization did not call the object constructor";

    ASSERT_EQ(test.GetObject().m_testFlag, validObjectFlag) << "Constructed object is invalid";
}

TEST(TestFunctional, TestAssignments) {
    bool unused = false;

    StorageTest test(unused);

    Storage<StorageTest> inPlace{InPlaceConstructor::Value, unused};
    Storage<StorageTest> copied{inPlace};
    Storage<StorageTest> moved{Move(inPlace)};
}

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