#include <FunnyOS/QuickFat/QuickFat.h>

/**
 * Size of a sector in bytes.
 */
#define SECTOR_SIZE 0x200

/**
 * Name of the file to boot
 */
#define FILE_NAME "BOOTLD32BIN"
#define FILE_DISPLAY_NAME "BOOTLD32.BIN"
#define FILE_MAGIC 0x46554E42

/**
 * Location of where to put the bootloader in memory.
 */
#define LOCATION 0x4000

// Boot parameters
extern uint8_t g_boot_partition;

/**
 * Prints string onto the screen.
 */
extern void fl_print(const char* str);

/**
 * Loads one sector from the boot drive starting from logical sector address [lba].
 *
 * @param[out] out pointer to output buffer will be put in here
 *
 * @return 0 if success, a non-zero error code if fail
 */
extern int fl_load_from_disk(uint32_t lba, uint8_t** out);

/**
 * Jumps back to real mode, loads boot parameters and jumps to address at [bootloader_location]
 */
extern void _Noreturn fl_jump_to_bootloader(void* bootloader_location);

/**
 * Hangs the CPU indefinitely.
 */
extern void _Noreturn fl_hang(void);

/**
 * Prints an integer onto the screen.
 */
void fl_print_int(unsigned int number) {
    const int count = sizeof(int) * 2;
    char buffer[count + 1];

    for (int i = 0; i < count; i++) {
        const unsigned char part = (char)(number >> ((count - i - 1) * 4)) & 0x0F;
        buffer[i] = "0123456789ABCDEF"[part];
    }
    buffer[count] = 0;

    fl_print(buffer);
}

/**
 * Prints an error code and hangs the CPU indefinitely.
 */
#define CHECK_ERROR(code, error) \
    if ((error_code & (code)) == (code)) fl_print((error))

void _Noreturn fl_error(unsigned int error_code) {
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_LOAD_MBR, " ** Failed to load the MBR into memory");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH, " ** The MBR signature is invalid");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_LOAD_BPB, " ** Failed to load the BPB into memory");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH, " ** The BPB signature is invalid");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP, " ** Failed to load cluster info from FAT");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_TO_LOAD_FAT_CLUSTER, " ** Failed to load cluster from disk");
    CHECK_ERROR(QUICKFAT_ERROR_FAILED_TO_FIND_FILE, " ** " FILE_NAME " not found.");

    fl_print("\r\n ** Loading error: 0x");
    fl_print_int(error_code);
    fl_print("\r\n");
    fl_hang();
}
#undef CHECK_ERROR

/**
 * Main function
 */
void _Noreturn fat_loader(void) {
    fl_print("FunnyOS FAT32 Loader\r\n");

    // Init context
    fl_print(" * Initializing the QuickFat library\r\n");
    QuickFat_initialization_data init;
    init.sector_size = SECTOR_SIZE;
    init.partition_entry = g_boot_partition;
    init.read_function = fl_load_from_disk;

    QuickFat_Context context;
    int error;

    if ((error = quickfat_init_context(&context, &init)) != 0) {
        fl_error(error);
    }

    // Open file
    fl_print(" * Searching for " FILE_DISPLAY_NAME "\r\n");

    QuickFat_File file;
    if ((error = quickfat_open_file(&context, &file, FILE_NAME)) != 0) {
        fl_error(error);
    }

    // Read file
    fl_print(" * File found. Loading... \n\r");
    if ((error = quickfat_read_file(&context, &file, (void*)LOCATION)) != 0) {
        fl_error(error);
    }

    // Verify
    fl_print(" * File loaded. Verifying... \n\r");
    uint32_t* magic_location = (uint32_t*)(((uint8_t*)LOCATION) + file.size - 4);

    // Die, horribly
    if (*magic_location != FILE_MAGIC) {
        fl_print(" ** Verification failed. \n\r");
        fl_error(0xFFFFFFFF);
    }

    fl_jump_to_bootloader((void*)LOCATION);
}