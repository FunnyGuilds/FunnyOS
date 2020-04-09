function(setup_tests name lib)
    add_executable(${name}
            StdlibPlatform.cpp
            TestAlgorithm.cpp
            TestFunctional.cpp
            TestMemory.cpp
            TestString.cpp
    )

    target_link_libraries(${name}
            PUBLIC
                ${lib}
                GTest::GTest
                GTest::Main
    )

    add_test(${name} ${name})
endfunction()

if (F_COMPILE64)
    setup_tests(FunnyOS_Stdlib_Base64_Tests FunnyOS::Stdlib::Base64_Static)
    add_executable(FunnyOS::Stdlib::Base64_Tests ALIAS FunnyOS_Stdlib_Base64_Tests)
else()
    setup_tests(FunnyOS_Stdlib_Base32_Tests FunnyOS::Stdlib::Base32_Static)

    set_target_properties(FunnyOS_Stdlib_Base32_TestsPROPERTIES
            LINK_FLAGS "-m32"
    )
    add_executable(FunnyOS::Stdlib::Base32_Tests ALIAS FunnyOS_Stdlib_Base32_Tests)
endif ()