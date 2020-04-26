set(F_BOOTLOADER_LINK_FLAGS "-nostartfiles;-nodefaultlibs;-nostdlib")

add_subdirectory("bootsector")
add_subdirectory("bootloader32")
add_subdirectory("env64")
add_subdirectory("temp_kernel")