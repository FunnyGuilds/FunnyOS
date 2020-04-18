#ifdef __GNUC__
#define PACKED __attribute__((packed))
#endif

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

/**
 * NULL is NULL
 */
#define NULL ((void*) 0)

#define ERROR_FAILED_LOAD_MBR 0xFF000000
#define ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH 0xFF100000
#define ERROR_FAILED_LOAD_BPB 0xFF020000
#define ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH 0xFF030000
#define ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP 0xFF040000
#define ERROR_FAILED_TO_LOAD_FAT_CLUSTER 0xFF050000
#define ERROR_FAILED_TO_FIND_FILE 0xFF060000
#define ERROR_FILE_NOT_ALIGNED 0xFF070000
#define ERROR_FAILED_TO_VERIFY_FILE 0xFF080000

#define MBR_SIGNATURE 0xAA55

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef struct {
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fats;
    uint32_t hidden_sectors;
    uint32_t sectors_per_fat;
    uint32_t root_directory_cluster;
    uint32_t fat_start_lba;
    uint32_t data_start_lba;
} preserved_fat_data;

typedef struct PACKED {
    uint8_t drive_attributes;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t first_lba;
    uint32_t sectors_count;
} MBR_partition_entry;

typedef struct PACKED {
    uint8_t code[446];
    MBR_partition_entry partition_entries[4];
    uint16_t boot_signature;
} MBR;

typedef struct PACKED {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fats_count;
    uint16_t directory_entries_count;
    uint16_t sector_totals;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat_16;
    uint16_t sectors_per_track;
    uint16_t heads_count;
    uint32_t hidden_sectors_count;
    uint32_t large_sectors_count;
    uint32_t sectors_per_fat_32;
    uint16_t flags;
    uint16_t fat_version_number;
    uint32_t root_directory_cluster;
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t driver_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_serial_number;
    char label[11];
    char system_identifier[8];
    uint8_t boot_code[420];
    uint16_t boot_signature;
} FAT32BPB;

#define FAT32_DIRECTORY_ENTRIES_PER_SECTOR (0x200 / 32)

typedef struct PACKED {
    char file_name_and_ext[8 + 3];
    uint8_t file_data_1[9];
    uint16_t cluster_num_high;
    uint8_t file_data_2[4];
    uint16_t cluster_num_low;
    uint32_t file_size_bytes;
} FAT_directory_entry;

// Boot parameters
extern uint8_t g_boot_drive;
extern uint8_t g_boot_partition;

// Data
preserved_fat_data g_data;

/**
 * Prints string onto the screen.
 */
extern void fl_print(const char* str);

/**
 * Loads one sector from drive [driveNumber] starting from logical sector address [lba].
 *
 * @param[out] out pointer to output buffer will be put in here
 * @return 0 if success, a non-zero error code if fail
 */
extern unsigned int fl_load_from_disk(uint32_t lba, uint8_t driveNumber, uint8_t** out);

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
void _Noreturn fl_error(unsigned int error_code) {
    fl_print(" ** Loading error: 0x");
    fl_print_int(error_code);
    fl_print("\r\n");
    fl_hang();
}

/**
 * Copies [size] bytes, from [source] to [destination]
 */
void fl_memcpy(void* destination, const void* source, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        *(((uint8_t*)destination) + i) = *(((uint8_t*)source) + i);
    }
}

/**
 * Checks if the two strings on size [size] are equal.
 *
 * @return 1 if strings are equal, 0 if not
 */
int fl_strequal(const char* str1, const char* str2, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }

    return 1;
}

/**
 * Loads the boot drive MBR to memory.
 */
MBR* fl_load_mbr() {
    uint8_t* buffer;
    int error;

    if ((error = fl_load_from_disk(0, g_boot_drive, &buffer)) != 0) {
        fl_print(" ** Failed to load the MBR into memory\r\n");
        fl_error(ERROR_FAILED_LOAD_MBR | error);
    }

    MBR* mbr = (MBR*)buffer;

    if (mbr->boot_signature != MBR_SIGNATURE) {
        fl_print(" ** The MBR signature is invalid\r\n");
        fl_error(ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH);
    }

    return mbr;
}

/**
 * Loads the FAT32 BPB to memory.
 *
 * @param lba lba of the sector containing the BPB
 */
FAT32BPB* fl_load_bpb(uint32_t lba) {
    uint8_t* buffer;
    int error;

    if ((error = fl_load_from_disk(lba, g_boot_drive, &buffer)) != 0) {
        fl_print(" ** Failed to load the BPB into memory\r\n");
        fl_error(ERROR_FAILED_LOAD_BPB | error);
    }

    FAT32BPB* bpb = (FAT32BPB*)buffer;
    if (bpb->boot_signature != MBR_SIGNATURE || (bpb->signature != 0x28 && bpb->signature != 0x29)) {
        fl_print(" ** The BPB signature is invalid\r\n");
        fl_error(ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH);
    }

    return bpb;
}

/**
 * Loads cluster entry [number] from the first FAT.
 */
uint32_t fl_load_fat_cluster_from_map(uint32_t number) {
    uint32_t sector = number / (SECTOR_SIZE / 4);
    uint32_t offset = number % (SECTOR_SIZE / 4);

    unsigned char* buffer;
    int error;

    if ((error = fl_load_from_disk(g_data.fat_start_lba + sector, g_boot_drive, &buffer)) != 0) {
        fl_print(" ** Failed to load cluster info from FAT\r\n");
        fl_error(ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP | error);
    }

    uint32_t* clusters = (uint32_t*)buffer;

    return clusters[offset] & 0x0FFFFFFF;
}

/**
 * Loads the entire FAT data cluster to memory.
 *
 * @return pointer to where the cluster was loaded
 */
uint8_t* fl_load_fat_cluster_to_memory(uint32_t cluster_number) {
    unsigned char* buffer;
    int error;

    const unsigned int cluster_lba = g_data.data_start_lba + (cluster_number - 2) * g_data.sectors_per_cluster;

    if ((error = fl_load_from_disk(cluster_lba, g_boot_drive, &buffer)) != 0) {
        fl_print(" ** Failed to load cluster from disk \r\n");
        fl_error(ERROR_FAILED_TO_LOAD_FAT_CLUSTER | error);
    }

    return (uint8_t*)buffer;
}

/**
 * Main function
 */
void _Noreturn fat_loader(void) {
    fl_print("FunnyOS FAT32 Loader\r\n");
    fl_print(" * Loading the MBR\r\n");
    MBR* mbr = fl_load_mbr();
    const uint32_t boot_partition_lba = mbr->partition_entries[g_boot_partition - 1].first_lba;

    fl_print(" * Loading the BPB\r\n");
    FAT32BPB* bpb = fl_load_bpb(boot_partition_lba);
    mbr = NULL;

    fl_print(" * Gathering FAT data\r\n");
    g_data.sectors_per_cluster = bpb->sectors_per_cluster;
    g_data.reserved_sectors = bpb->reserved_sectors;
    g_data.number_of_fats = bpb->fats_count;
    g_data.hidden_sectors = bpb->hidden_sectors_count;
    g_data.sectors_per_fat = bpb->sectors_per_fat_32;
    g_data.root_directory_cluster = bpb->root_directory_cluster;
    g_data.fat_start_lba = boot_partition_lba + bpb->reserved_sectors + bpb->hidden_sectors_count;
    g_data.data_start_lba = g_data.fat_start_lba + bpb->fats_count * bpb->sectors_per_fat_32;
    bpb = NULL;

    fl_print(" * Searching for " FILE_DISPLAY_NAME "\r\n");

    uint32_t current_cluster_number = g_data.root_directory_cluster;
    FAT_directory_entry* file_entry = NULL;

    do {
        FAT_directory_entry* directory_entries =
            (FAT_directory_entry*)fl_load_fat_cluster_to_memory(current_cluster_number);

        for (unsigned int i = 0; i < FAT32_DIRECTORY_ENTRIES_PER_SECTOR; i++) {
            if (directory_entries[i].file_name_and_ext[0] == 0x00) {
                // no more entries here
                break;
            }

            if (fl_strequal(directory_entries[i].file_name_and_ext, FILE_NAME, 8 + 3)) {
                file_entry = &directory_entries[i];
                break;
            }
        }

        if (file_entry != NULL) {
            break;
        }

        current_cluster_number = fl_load_fat_cluster_from_map(current_cluster_number);
    } while (current_cluster_number >= 0x00000002 && current_cluster_number <= 0x0FFFFEF);

    if (file_entry == NULL) {
        fl_print(" ** " FILE_DISPLAY_NAME " not found.\r\n");
        fl_error(ERROR_FAILED_TO_FIND_FILE);
    }

    fl_print(" * File found. Loading... \n\r");
    if (file_entry->file_size_bytes % SECTOR_SIZE != 0) {
        fl_print(" ** File is not sector-aligned.\n\r");
        fl_error(ERROR_FILE_NOT_ALIGNED);
    }

    const uint32_t file_cluster = file_entry->cluster_num_high << 16 | file_entry->cluster_num_low;
    const uint32_t file_size_in_sectors = file_entry->file_size_bytes / SECTOR_SIZE;
    file_entry = NULL;

    uint8_t* destination = (uint8_t*)LOCATION;
    for (uint32_t i = 0; i < file_size_in_sectors; i++) {
        uint8_t* data = fl_load_fat_cluster_to_memory(file_cluster + i);
        fl_memcpy(destination, data, SECTOR_SIZE);
        destination += SECTOR_SIZE;
    }

    fl_print(" * File loaded. Verifying... \n\r");
    uint32_t* magic_location = (uint32_t*)(destination - 4);
    if (*magic_location != FILE_MAGIC) {
        fl_print(" ** Verification failed. \n\r");
        fl_error(ERROR_FAILED_TO_VERIFY_FILE);
    }

    fl_jump_to_bootloader((void*)LOCATION);
}