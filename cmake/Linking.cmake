#
# Links the target using the given linker script
#
function(custom_link target script)
    set_target_properties(${target} PROPERTIES LINKER_LANGUAGE "CXX")
    target_link_options(${target} PRIVATE -T "${script}")
endfunction()

#
# Sets the target output filename and suffix
#
function(target_output_filename target filename)
    string(REPLACE "." ";" filename_list "${filename}")
    list(GET filename_list 0 filename_only)
    list(REMOVE_AT filename_list 0)
    list(JOIN filename_list "." filename_suffix)
    if (NOT "${filename_suffix}" STREQUAL "")
        set(filename_suffix ".${filename_suffix}")
    endif ()

    set_target_properties(${target} PROPERTIES
            PREFIX ""
            OUTPUT_NAME "${filename_only}"
            SUFFIX "${filename_suffix}"
    )
endfunction()