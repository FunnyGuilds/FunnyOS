add_library(FunnyOS_Misc_Bootparams INTERFACE)
target_include_directories(FunnyOS_Misc_Bootparams
        INTERFACE
            "${CMAKE_CURRENT_SOURCE_DIR}/headers"
)

# We don't link to stdlib because this is a header-only library and the actual target may choose any stdlib variant it