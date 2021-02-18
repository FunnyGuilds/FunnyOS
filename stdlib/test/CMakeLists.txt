add_executable(FunnyOS_Stdlib_Base_Tests
        StdlibPlatform.cpp
        TestAlgorithm.cpp
        TestDynamicString.cpp
        TestFunctional.cpp
        TestLinkedList.cpp
        TestMemory.cpp
        TestString.cpp
        TestUtility.cpp
        TestVector.cpp
)

target_link_libraries(FunnyOS_Stdlib_Base_Tests
        PUBLIC
            FunnyOS_Stdlib_Base_Static_Test
            GTest::GTest
            GTest::Main
)
