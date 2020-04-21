add_library(FunnyOS_Misc_Bootparams INTERFACE)
target_include_directories(FunnyOS_Misc_Bootparams INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/headers")

add_library(FunnyOS::Misc::Bootparams ALIAS FunnyOS_Misc_Bootparams)
