set(F_BOOTLOADER_LINK_FLAGS "-nostartfiles;-nodefaultlibs;-nostdlib")

add_subdirectory("bootsector")
add_subdirectory("bootloader32")