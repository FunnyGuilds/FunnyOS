#include <FunnyOS/QuickFat/QuickFat.h>

#define QUICK_FAT_LFN_ATTR 0x0F
#define QUICK_FAT_SECTOR_SIZE 0x200

typedef struct PACKED {
    uint8_t drive_attributes;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t first_lba;
    uint32_t sectors_count;
} QuickFat_MBR_partition_entry;

typedef struct PACKED {
    uint8_t code[446];
    QuickFat_MBR_partition_entry partition_entries[4];
    uint16_t boot_signature;
} QuickFat_MBR;

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
} QuickFat_FAT32BPB;

typedef struct PACKED {
    char file_name_and_ext[8 + 3];
    uint8_t attribute;
    uint8_t file_data_1[8];
    uint16_t cluster_num_high;
    uint8_t file_data_2[4];
    uint16_t cluster_num_low;
    uint32_t file_size_bytes;
} QuickFat_directory_entry;

typedef struct PACKED {
    uint8_t sequence_number;
    char name1[10];
    uint8_t attribute;
    uint8_t type;
    uint8_t dos_checksum;
    char name2[12];
    uint16_t first_cluster;
    char name3[4];
} QuickFat_lfn_entry;

extern void memcpy(void* destination, const void* source, size_t size);

static int quickfat_do_read(QuickFat_Context* context, uint32_t lba, uint32_t count, uint8_t* out) {
    return context->read_function(context->read_function_data, lba, count, out);
}

static uint8_t g_clusterBuffer[QUICK_FAT_SECTOR_SIZE * 4];
static uint32_t clusters[QUICK_FAT_SECTOR_SIZE / sizeof(uint32_t)];
static uint32_t last_cluster_sector = 0xFFFFFFFF;

int quickfat_init_context(QuickFat_Context* context, const QuickFat_initialization_data* init) {
    context->read_function = init->read_function;
    context->read_function_data = init->read_function_data;

    int error;

    if ((error = quickfat_do_read(context, 0, 1, (uint8_t*)&g_clusterBuffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_LOAD_MBR | error;
    }

    QuickFat_MBR* mbr = (QuickFat_MBR*)&g_clusterBuffer;

    if (mbr->boot_signature != QUICKFAT_MBR_SIGNATURE) {
        return QUICKFAT_ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH;
    }

    context->partition_start_lba = mbr->partition_entries[init->partition_entry - 1].first_lba;

    if ((error = quickfat_do_read(context, context->partition_start_lba, 1, (uint8_t*)&g_clusterBuffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_LOAD_BPB | error;
    }

    QuickFat_FAT32BPB* bpb = (QuickFat_FAT32BPB*)&g_clusterBuffer;
    if (bpb->boot_signature != QUICKFAT_MBR_SIGNATURE || (bpb->signature != 0x28 && bpb->signature != 0x29)) {
        return QUICKFAT_ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH;
    }

    if (bpb->sectors_per_cluster > 4) {
        return QUICKFAT_ERROR_FAILED_LOAD_BPB | 0xFF;
    }

    context->sectors_per_cluster = bpb->sectors_per_cluster;
    context->entries_per_cluster = (context->sectors_per_cluster * QUICK_FAT_SECTOR_SIZE) / 32;
    context->reserved_sectors = bpb->reserved_sectors;
    context->number_of_fats = bpb->fats_count;
    context->hidden_sectors = bpb->hidden_sectors_count;
    context->sectors_per_fat = bpb->sectors_per_fat_32;
    context->root_directory_cluster = bpb->root_directory_cluster;
    context->fat_start_lba = context->partition_start_lba + bpb->reserved_sectors + bpb->hidden_sectors_count;
    context->data_start_lba = context->fat_start_lba + bpb->fats_count * bpb->sectors_per_fat_32;
    return 0;
}

/**
 * Copies [size] LFN characters from [destination] to [source], ignores high bit
 */
static void quickfat_lfncpy(void* destination, const void* source, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        // ignore high bytes
        *(((uint8_t*)destination) + i) = *(((uint8_t*)source) + (i * 2));
    }
}

/**
 * Checks if the two strings are equal.
 *
 * @return 1 if strings are equal, 0 if not
 */
static int quickfat_strequal(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*(str1++) != *(str2++)) {
            return 0;
        }
    }

    return *str1 == *str2;
}

/**
 * Checks if the two strings on size [size] are equal.
 *
 * @return 1 if strings are equal, 0 if not
 */
static int quickfat_strnequal(const char* str1, const char* str2, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (!str1[0] || !str2[1] || str1[i] != str2[i]) {
            return 0;
        }
    }

    return 1;
}

/**
 * Loads cluster entry [number] from the first FAT.
 */
static int quickfat_load_fat_cluster_from_map(QuickFat_Context* context, uint32_t number, uint32_t* out) {
    uint32_t sector = number / (QUICK_FAT_SECTOR_SIZE / sizeof(uint32_t));
    uint32_t offset = number % (QUICK_FAT_SECTOR_SIZE / sizeof(uint32_t));

    if (last_cluster_sector != sector) {
        int error;
        if ((error = quickfat_do_read(context, context->fat_start_lba + sector, 1, (uint8_t*)clusters)) != 0) {
            return QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP | error;
        }

        last_cluster_sector = sector;
    }

    *out = clusters[offset] & 0x0FFFFFFF;
    return 0;
}

/**
 * Loads the entire FAT data cluster to memory.
 *
 * @return pointer to where the cluster was loaded
 */
static int quickfat_load_fat_cluster_to_memory(QuickFat_Context* context, uint32_t cluster_number, uint32_t count,
                                               uint8_t* buffer) {
    int error;
    uint32_t buffer_offset = 0;

    while (count > 0) {
        uint32_t current_read = 1;

        // find how many consecutive clusters there are
        for (size_t i = 0; i < count - 1; i++) {
            uint32_t next_cluster;

            if ((error = quickfat_load_fat_cluster_from_map(context, cluster_number + i, &next_cluster)) != 0) {
                return QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP | error;
            }

            if (next_cluster != cluster_number + i + 1) {
                break;
            }

            current_read++;
        }

        // do read
        const uint32_t cluster_lba = context->data_start_lba + (cluster_number - 2) * context->sectors_per_cluster;
        const uint32_t last_read_cluster = cluster_number + current_read - 1;

        if ((error = quickfat_do_read(context, cluster_lba, current_read * context->sectors_per_cluster,
                                      buffer + buffer_offset)) != 0) {
            return QUICKFAT_ERROR_FAILED_TO_LOAD_FAT_CLUSTER | error;
        }

        // fetch next cluster
        if ((error = quickfat_load_fat_cluster_from_map(context, last_read_cluster, &cluster_number)) != 0) {
            return QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP | error;
        }

        count -= current_read;
        buffer_offset += current_read * context->sectors_per_cluster * QUICK_FAT_SECTOR_SIZE;

        if (cluster_number < 0x00000002 || cluster_number > 0x0FFFFEF) {
            if (count == 0) {
                return 0;
            }

            return QUICKFAT_ERROR_FAILED_TO_LOAD_FAT_CLUSTER | 0xFF;
        }
    }

    return 0;
}

int quickfat_open_file_in(QuickFat_Context* context, QuickFat_File* directory, QuickFat_File* file, const char* name) {
    int error;
    uint32_t current_cluster_number = directory->cluster;
    QuickFat_directory_entry* file_entry = 0;

    char current_lfn[13 * 5 + 1] = {0};
    bool has_lfn = false;

    do {
        error = quickfat_load_fat_cluster_to_memory(context, current_cluster_number, 1, (uint8_t*)&g_clusterBuffer);
        if (error != 0) {
            return error;
        }

        QuickFat_directory_entry* directory_entries = (QuickFat_directory_entry*)&g_clusterBuffer;

        for (unsigned int i = 0; i < context->entries_per_cluster; i++) {
            if (directory_entries[i].file_name_and_ext[0] == 0x00) {
                // no more entries here
                break;
            }

            if (directory_entries[i].attribute == QUICK_FAT_LFN_ATTR) {
                has_lfn = true;

                QuickFat_lfn_entry* lfn = (QuickFat_lfn_entry*)&directory_entries[i];

                if (lfn->sequence_number & 0b01000000) {  // Is first entry in the table
                    for (unsigned int j = 0; j < sizeof(current_lfn) / sizeof(current_lfn[0]); j++) {
                        current_lfn[j] = ' ';
                    }
                }

                const unsigned int lfn_index = ((lfn->sequence_number & 0b00011111) - 1U) * 13U;
                if (lfn_index >= 5 * 13) {
                    continue;
                }

                quickfat_lfncpy(current_lfn + lfn_index + 00, lfn->name1, 5);
                quickfat_lfncpy(current_lfn + lfn_index + 05, lfn->name2, 6);
                quickfat_lfncpy(current_lfn + lfn_index + 11, lfn->name3, 2);
                continue;
            }

            if (has_lfn) {
                // remove trailing spaces
                for (int j = sizeof(current_lfn) / sizeof(current_lfn[0]) - 2; j >= -1; j--) {
                    if (j == -1 || current_lfn[j] != ' ') {
                        current_lfn[j + 1] = 0;
                        break;
                    }
                }

                // null byte
                current_lfn[sizeof(current_lfn) - 1] = 0;
            }

            if ((has_lfn && quickfat_strequal(current_lfn, name)) ||
                quickfat_strnequal(directory_entries[i].file_name_and_ext, name, 8 + 3)) {
                file_entry = &directory_entries[i];
                break;
            }

            if (has_lfn) {
                has_lfn = false;
            }
        }

        if (file_entry != 0) {
            break;
        }

        error = quickfat_load_fat_cluster_from_map(context, current_cluster_number, &current_cluster_number);
        if (error != 0) {
            return error;
        }
    } while (current_cluster_number >= 0x00000002 && current_cluster_number <= 0x0FFFFEF);

    if (file_entry == 0) {
        return QUICKFAT_ERROR_FAILED_TO_FIND_FILE;
    }

    file->cluster = file_entry->cluster_num_high << 16 | file_entry->cluster_num_low;
    file->size = file_entry->file_size_bytes;
    file->cluster_size = file_entry->file_size_bytes / (QUICK_FAT_SECTOR_SIZE * context->sectors_per_cluster);

    if ((file_entry->file_size_bytes % (QUICK_FAT_SECTOR_SIZE * context->sectors_per_cluster)) != 0) {
        file->cluster_size++;
    }

    return 0;
}

int quickfat_open_file(QuickFat_Context* context, QuickFat_File* file, const char* file_path) {
    int error;
    unsigned int current_index = 0;
    char current_part[128];
    QuickFat_File current_directory;
    current_directory.cluster = context->root_directory_cluster;

    // Skip trailing slashes
    while (file_path[current_index] == '/') {
        current_index++;
    }

    // Walk the directory tree
    for (;;) {
        bool expect_directory = true;

        for (unsigned int i = 0; i < sizeof(current_part) / sizeof(current_part[0]); i++) {
            if (file_path[i + current_index] == 0) {
                memcpy(current_part, file_path + current_index, i);
                current_part[i] = 0;
                expect_directory = false;
                break;
            }

            if (file_path[i + current_index] == '/') {
                memcpy(current_part, file_path + current_index, i);
                current_part[i] = 0;
                current_index += i + 1;
                expect_directory = true;
                break;
            }
        }

        if ((error = quickfat_open_file_in(context, &current_directory, file, current_part)) != 0) {
            return error;
        }

        if (expect_directory) {
            current_directory = *file;
        } else {
            return 0;
        }
    }
}

int quickfat_read_file(QuickFat_Context* context, QuickFat_File* file, void* destination) {
    int error;

    // File is cluster-aligned
    if ((file->size % (context->sectors_per_cluster * QUICK_FAT_SECTOR_SIZE)) == 0) {
        return quickfat_load_fat_cluster_to_memory(context, file->cluster, file->cluster_size, destination);
    }

    // Read whatever is aligned
    const uint32_t initial_sector_read_size = file->cluster_size - 1;
    if (initial_sector_read_size > 0) {
        error = quickfat_load_fat_cluster_to_memory(context, file->cluster, initial_sector_read_size, destination);

        if (error != 0) {
            return error;
        }

        destination += QUICK_FAT_SECTOR_SIZE * initial_sector_read_size;
    }

    // Read the rest
    error = quickfat_load_fat_cluster_to_memory(context, file->cluster + initial_sector_read_size, 1,
                                                (uint8_t*)&g_clusterBuffer);
    if (error != 0) {
        return error;
    }

    memcpy(destination, &g_clusterBuffer, file->size % QUICK_FAT_SECTOR_SIZE);

    return error;
}