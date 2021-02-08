;
; Stage1 Bootloader code
;
%include "config.asm"

[bits 16]
EXTERN load_lba
EXTERN print
EXTERN error

SECTION .bios_parameter_block
    ; Bios Parameter Block
    ; First 3 bytes E9 XX 90
    jmp short intro
    nop

    ; The rest of the BPB will be put by a formatting tool

SECTION .intro
    intro:
        ; Reset CS
        jmp     0x00:stage1

SECTION .text
    stage1:
        ; We don't need any interrupts at this stage
        cli

        ; Clear segment registers
        xor     ax, ax
        mov     ds, ax
        mov     es, ax

        ; Setup stack
        mov     ss, ax
        mov     sp, 0x7C00
        mov     bp, sp

        ; Load MBR at 0x500
        mov     eax, 0
        mov     cl, 1
        mov     bx, 0x500
        call    load_lba
        int     0x13
        jc      error

        mov     si, booting_message
        call    print

        ; Find the drive we were booted from
        mov     dh, 1                   ; partition number
        mov     bx, 0x500 + 0x1BE       ; partition entry base, 0x1BE - start of partition table
        mov     cx, 4

    .loop:
        test    byte [bx], 0x80
        jnz     bootable_drive_found
        add     bx, 0x10
        inc     dh
        loop    .loop

        ; No drive found
        mov     ah, 0xFF
        jmp     error

    bootable_drive_found:
        ; Load the bootloader
        mov     eax, [bx + 0x08]
        add     eax, 2

        mov     bx, F_FATLOADER_MEMORY_LOCATION
        mov     cl, F_FATLOADER_SIZE_IN_SECTORS
        call    load_lba
        jc      error

        ; Magic check
        mov     ah, 0xFD
        cmp     DWORD [F_FATLOADER_MEMORY_LOCATION + F_FATLOADER_SIZE_IN_SECTORS * 0x200 - 4], F_FATLOADER_MAGIC
        jne     error

        ; Jump to bootloader
        ; DL - drive number
        ; DH - partition number
        jmp 0x00:F_FATLOADER_MEMORY_LOCATION

SECTION .rodata
    booting_message:     db "FunnyOS Bootloader", 0

SECTION .boot_signature
    dw 0xAA55
