#include <FunnyOS/QuickFat/QuickFat.h>

//
// TODO:
// - LFN support
// - subdirectories support
//

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
    uint8_t file_data_1[9];
    uint16_t cluster_num_high;
    uint8_t file_data_2[4];
    uint16_t cluster_num_low;
    uint32_t file_size_bytes;
} QuickFat_directory_entry;


int quickfat_init_context(QuickFat_Context* context, const QuickFat_initialization_data* init) {
    context->read_function = init->read_function;
    context->sector_size = init->sector_size;
    context->entries_per_sector = context->sector_size / 32;

    uint8_t* buffer;
    int error;

    if ((error = init->read_function(0, &buffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_LOAD_MBR | error;
    }

    QuickFat_MBR* mbr = (QuickFat_MBR*)buffer;

    if (mbr->boot_signature != QUICKFAT_MBR_SIGNATURE) {
        return QUICKFAT_ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH;
    }

    context->partition_start_lba = mbr->partition_entries[init->partition_entry - 1].first_lba;
    mbr = 0;

    if ((error = context->read_function(context->partition_start_lba, &buffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_LOAD_BPB | error;
    }

    QuickFat_FAT32BPB* bpb = (QuickFat_FAT32BPB*)buffer;
    if (bpb->boot_signature != QUICKFAT_MBR_SIGNATURE || (bpb->signature != 0x28 && bpb->signature != 0x29)) {
        return QUICKFAT_ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH;
    }

    context->sectors_per_cluster = bpb->sectors_per_cluster;
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
 * Copies [size] bytes, from [source] to [destination]
 */
static void quickfat_memcpy(void* destination, const void* source, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        *(((uint8_t*)destination) + i) = *(((uint8_t*)source) + i);
    }
}

/**
 * Checks if the two strings on size [size] are equal.
 *
 * @return 1 if strings are equal, 0 if not
 */
static int quickfat_strequal(const char* str1, const char* str2, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }

    return 1;
}

/**
 * Loads cluster entry [number] from the first FAT.
 */
static int quickfat_load_fat_cluster_from_map(QuickFat_Context* context, uint32_t number, uint32_t* out) {
    uint32_t sector = number / (context->sector_size / 4);
    uint32_t offset = number % (context->sector_size / 4);

    unsigned char* buffer;
    int error;

    if ((error = context->read_function(context->fat_start_lba + sector, &buffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP | error;
    }

    uint32_t* clusters = (uint32_t*)buffer;

    *out = clusters[offset] & 0x0FFFFFFF;
    return 0;
}

/**
 * Loads the entire FAT data cluster to memory.
 *
 * @return pointer to where the cluster was loaded
 */
static int quickfat_load_fat_cluster_to_memory(QuickFat_Context* context, uint32_t cluster_number, uint8_t** buffer) {
    int error;

    const unsigned int cluster_lba = context->data_start_lba + (cluster_number - 2) * context->sectors_per_cluster;

    if ((error = context->read_function(cluster_lba, buffer)) != 0) {
        return QUICKFAT_ERROR_FAILED_TO_LOAD_FAT_CLUSTER | error;
    }

    return 0;
}

int quickfat_open_file(QuickFat_Context* context, QuickFat_File* file, const char* fileName) {
    uint8_t* buffer;
    int err;
    uint32_t current_cluster_number = context->root_directory_cluster;
    QuickFat_directory_entry* file_entry = 0;

    do {
        if ((err = quickfat_load_fat_cluster_to_memory(context, current_cluster_number, &buffer)) != 0) {
            return err;
        }

        QuickFat_directory_entry* directory_entries = (QuickFat_directory_entry*)buffer;

        for (unsigned int i = 0; i < context->entries_per_sector; i++) {
            if (directory_entries[i].file_name_and_ext[0] == 0x00) {
                // no more entries here
                break;
            }

            if (quickfat_strequal(directory_entries[i].file_name_and_ext, fileName, 8 + 3)) {
                file_entry = &directory_entries[i];
                break;
            }
        }

        if (file_entry != 0) {
            break;
        }

        if ((err = quickfat_load_fat_cluster_from_map(context, current_cluster_number, &current_cluster_number)) != 0) {
            return err;
        }
    } while (current_cluster_number >= 0x00000002 && current_cluster_number <= 0x0FFFFEF);

    if (file_entry == 0) {
        return QUICKFAT_ERROR_FAILED_TO_FIND_FILE;
    }

    file->cluster = file_entry->cluster_num_high << 16 | file_entry->cluster_num_low;
    file->size = file_entry->file_size_bytes;
    file->sector_size = file_entry->file_size_bytes / context->sector_size;
    if ((file_entry->file_size_bytes % context->sector_size) != 0) {
        file->sector_size++;
    }

    return 0;
}

int quickfat_read_file(QuickFat_Context* context, QuickFat_File* file, void* destination) {
    int error;
    uint8_t* buffer;

    for (uint32_t i = 0; i < file->sector_size; i++) {
        if ((error = quickfat_load_fat_cluster_to_memory(context, file->cluster + i, &buffer)) != 0) {
            return error;
        }
        uint32_t copy_size = context->sector_size;
        if (i == file->sector_size - 1 && file->size % context->sector_size != 0) {
            copy_size = file->size % context->sector_size;
        }

        quickfat_memcpy(destination, buffer, copy_size);
        destination += copy_size;
    }

    return 0;
}