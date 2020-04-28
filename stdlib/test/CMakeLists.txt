add_executable(FunnyOS_Stdlib_Base64_Tests
        StdlibPlatform.cpp
        TestAlgorithm.cpp
        TestFunctional.cpp
        TestLinkedList.cpp
        TestMemory.cpp
        TestString.cpp
        TestUtility.cpp
        TestVector.cpp
)

target_link_libraries(FunnyOS_Stdlib_Base64_Tests
        PUBLIC
            FunnyOS_Stdlib_Base64_Static_Test
            GTest::GTest
            GTest::Main
)
