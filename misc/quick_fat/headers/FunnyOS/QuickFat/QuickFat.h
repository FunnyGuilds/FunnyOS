#ifndef FUNNYOS_MISC_QUICK_FAT_QUICK_FAT_H
#define FUNNYOS_MISC_QUICK_FAT_QUICK_FAT_H

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#error "Unsupported compiler"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FOS_QUICKFAT_NO_FREESTANDING_HEADERS
#include <stddef.h>
#include <stdbool.h>
#endif

#ifndef FOS_QUICKFAT_NO_NUMERAL_TYPES
#include <stdint.h>
#endif

#define QUICKFAT_MBR_SIGNATURE 0xAA55

#define QUICKFAT_ERROR_FAILED_LOAD_MBR 0xFF100000
#define QUICKFAT_ERROR_FAILED_LOAD_MBR_SIGNATURE_MISMATCH 0xFF200000
#define QUICKFAT_ERROR_FAILED_LOAD_BPB 0xFF040000
#define QUICKFAT_ERROR_FAILED_LOAD_BPB_SIGNATURE_MISMATCH 0xFF100000
#define QUICKFAT_ERROR_FAILED_TO_LOAD_CLUSTER_FROM_MAP 0xFF200000
#define QUICKFAT_ERROR_FAILED_TO_LOAD_FAT_CLUSTER 0xFF400000
#define QUICKFAT_ERROR_FAILED_TO_FIND_FILE 0xFF800000

/**
 * Pointer to a function that loads [count] sectors, starting at [lba] from the boot drive into memory location [out]
 *
 * @return 0 on success, non-zero error code on fail
 */
typedef int (*QuickFat_ReadFunction)(void* data, uint32_t lba, uint32_t count, uint8_t* out);

/**
 * QuickFat context. Must be initialized via quickfat_init_context
 */
typedef struct {
    QuickFat_ReadFunction read_function;
    void* read_function_data;
    uint32_t partition_start_lba;
    uint32_t entries_per_cluster;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fats;
    uint32_t hidden_sectors;
    uint32_t sectors_per_fat;
    uint32_t root_directory_cluster;
    uint32_t fat_start_lba;
    uint32_t data_start_lba;
} QuickFat_Context;

/**
 * Data for quickfat_init_context.
 */
typedef struct {
    /**
     * Partition entry number of the bootable partition in the MBR.
     *
     * Value must be in range 1 to 4
     */
    uint32_t partition_entry;

    /**
     * Pointer to a function that will be used to read data from a drive.
     */
    QuickFat_ReadFunction read_function;

    /**
     * Argument to be passed to the read function on each call.
     */
    void* read_function_data;
} QuickFat_initialization_data;

/**
 * A single file description.
 */
typedef struct {
    /**
     * Cluster where the file starts
     */
    uint32_t cluster;

    /**
     * File size in bytes
     */
    uint32_t size;

    /**
     * File size in clusters.
     */
    uint32_t cluster_size;
} QuickFat_File;

/**
 * Initializes the given [context] with data supplied in [init]
 *
 * @return 0 on success, non-zero error code on fail
 */
int quickfat_init_context(QuickFat_Context* context, const QuickFat_initialization_data* init);

/**
 * Reads a file description from disk.
 *
 * @param context initialized QuickFat_Context
 * @param file file description struct to write to
 * @param file_path absolute path of the file, may contain trailing /
 *
 * @return 0 on success, non-zero error code on fail
 */
int quickfat_open_file(QuickFat_Context* context, QuickFat_File* file, const char* file_path);

/**
 * Reads a file into memory.
 *
 * @param context initialized QuickFat_Context
 * @param file file description, initialized by quickfat_open_file
 * @param destination memory location of where to read the file.
 * @return 0 on success, non-zero error code on fail
 */
int quickfat_read_file(QuickFat_Context* context, QuickFat_File* file, void* destination);

#ifdef __cplusplus
}
#endif

#endif  // FUNNYOS_MISC_QUICK_FAT_QUICK_FAT_H
