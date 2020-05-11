[bits 32]

SECTION .text
    EXTERN bootloader32_main

    GLOBAL protected_mode
    protected_mode:
        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov esp, 0x00080000
        mov ebp, esp

        ; Jump to C code
        jmp bootloader32_main