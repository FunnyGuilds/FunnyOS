OUTPUT_DIR=.

ASM = nasm
ASM_OPTS = -f bin

STAGE1_FILES = stage1/stage1.asm
STAGE2_FILES = stage2/stage2.asm stage2/a20line.asm stage2/logging.asm stage2/drive.asm stage2/gdt.asm \
	stage2/protected/protected.asm stage2/protected/screen.asm stage2/protected/paging.asm \
	stage2/protected/gdt.asm stage2/protected/long.asm

COMMON_FILES = common.asm data.asm

all: bootloader.img
.PHONY: all clean stage1.bin stage2.bin bootloader.bin

stage1.bin: $(STAGE1_FILES) $(COMMON_FILES)
	$(ASM) $(ASM_OPTS) -o $(OUTPUT_DIR)/stage1.bin stage1/stage1.asm

stage2.bin: $(STAGE2_FILES) $(COMMON_FILES)
	$(ASM) $(ASM_OPTS) -f bin -o $(OUTPUT_DIR)/stage2.bin stage2/stage2.asm

bootloader.img: stage1.bin stage2.bin
	cat $(OUTPUT_DIR)/stage1.bin >  $(OUTPUT_DIR)/bootloader.img
	cat $(OUTPUT_DIR)/stage2.bin >> $(OUTPUT_DIR)/bootloader.img

clean:
	rm -rf $(OUTPUT_DIR)/stage1.bin $(OUTPUT_DIR)/stage2.bin $(OUTPUT_DIR)/bootloader.img