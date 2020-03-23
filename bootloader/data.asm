%define MBR_BOOT_MAX_SIZE          512           ; Maximum size for a boot sector
%define BOOTLOADER_START_ADDRESS   0x7c00        ; Physical address at where the bootloader will be loaded

%define STAGE2_MAGIC               0xBADEEEFE
%define STAGE2_LOCATION            0x7E00
%define STAGE2_SIZE_SECTORS        0x08

%define KLD_STARTING_SECTOR        (STAGE2_SIZE_SECTORS + 2)                           ; 1 sector for stage1
%define KLD_SECTORS                16                                                  ; size of kernel loader in sectors
%define KLD_LAST_SECTOR            (KLD_STARTING_SECTOR + KLD_SECTORS - 1)             ; last sector of the kernel loader
%define KLD_SIZE                   (KLD_SECTORS * 0x200)                               ; 16 sectors, 8 KiB
%define KLD_ADDRESS                (STAGE2_LOCATION + STAGE2_SIZE_SECTORS * 0x200)     ; 0x8E00

;
; Checks
;

%if (KLD_SIZE % 0x200) != 0
    %fatal "KLD_SIZE is not divisbible by a sector size (512 bytes)"
%endif

%if (KLD_ADDRESS % 0x10) != 0
    %fatal "KLD_ADDRESS is not segment-aligned"
%endif


