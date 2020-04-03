;
; Stage1 Bootloader code
;
%define BOOTLOADER_SIZE_IN_SECTORS      30
%define BOOTLOADER_MAGIC                0x46554E42


[bits 16]

SECTION .bios_parameter_block
    ; Bios Parameter Block
    ; First 3 bytes E9 XX 90
    jmp short intro
    nop

    ; The rest of the BPB will be put by a formatting tool

SECTION .intro
    EXTERN load_lba
    EXTERN print
    EXTERN error

    intro:
        ; We don't need any interrupts at this stage
        cli

        ; Clear segment registers
        xor ax, ax
        mov ds, ax
        mov es, ax

        ; Setup stack
        mov ss, ax
        mov sp, 0x7C00
        mov bp, sp

        ; Find the partition we were booted from
        mov ah, 0x02
        mov al, 1
        mov ch, 0
        mov cl, 1
        mov dh, 0
        mov bx, 0x500
        int 0x13
        jc error

        mov si, message_booting
        call print

        ; Find the drive we were booted from
        mov bx,  0x500 + 0x1BE ; 1BE - start of partition table
        mov ch, 1

        find_bootable_drive_loop:
            mov ax, [bx]
            test ax, 0x80
            jnz bootable_drive_found
            add bx, 0x10
            cmp ch, 4
            jl find_bootable_drive_loop
            inc ch

        ; No drive found
        mov ah, 0xFF
        jmp error

    bootable_drive_found:
        ; Load the bootloader
        mov si, bx
        add si, 0x08
        mov eax, [si]
        add eax, 2
        mov [bootloader_lbs], eax

        mov si, bootloader_lbs
        mov bx, 0x500
        mov cl, BOOTLOADER_SIZE_IN_SECTORS
        call load_lba
        jc error

        ; Magic check
        mov ah, 0xFD
        cmp dword [0x500 + BOOTLOADER_SIZE_IN_SECTORS * 0x200 - 4], BOOTLOADER_MAGIC
        jne error

        ; Jump to bootloader
        jmp 0x00:0x500

SECTION .rodata
    message_booting:     db "FunnyOS Bootloader", 0
    message_boot_failed: db "Boot failed. ", 0

SECTION .bss
    bootloader_lbs: resd 1

SECTION .boot_signature
    dw 0xAA55