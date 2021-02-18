# OS Version
set(FUNNYOS_VERSION 0.1.0)

# Bootloader consts
set(F_FATLOADER_SIZE_IN_SECTORS       26)
set(F_FATLOADER_MAGIC                 0x464E5546)
set(F_FATLOADER_MEMORY_LOCATION       0x500)

set(F_BOOTLOADER_EXE_FILE_PATH        "/boot/bootload64")
set(F_BOOTLOADER_INI_FILE_PATH        "/boot/bootload64.ini")

set(F_BOOTLOADER_SIZE_IN_SECTORS      512)
set(F_BOOTLOADER_MAGIC                0x424E5546)
set(F_BOOTLOADER_MEMORY_LOCATION      0x4000)

# Kernel
set(F_KERNEL_FILE_PATH                     "/system/fkrnl.fxe")
set(F_KERNEL_VIRTUAL_ADDRESS                0xFFFF800000000000)
set(F_KERNEL_PHYSICAL_MAPPING_ADDRESS       0xFFFFA00000000000)
set(F_KERNEL_STACK_SIZE_KB                  16)
set(F_KERNEL_INITIAL_HEAP_SIZE_KB           4096)
