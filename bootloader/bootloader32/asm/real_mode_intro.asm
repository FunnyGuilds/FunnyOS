[bits 16]

SECTION .intro
    EXTERN protected_mode
    EXTERN make_memory_map

    intro:
        ; Setup bootloader_parameters
        mov [bootloader_parameters__boot_drive_id], dl
        mov [bootloader_parameters__boot_parttition], ch

        call make_memory_map
        mov [bootloader_parameters__acpi_extended], al
        mov [bootloader_parameters__memory_map_start], ebx
        mov [bootloader_parameters__memory_map_count], cx

        ; Setup video mode
        mov ah, 0x00
        mov al, 0x03
        int 0x10

        ; Setup protected mode
        lgdt [gdt_description]

        ; Disable interrupts and NMI
        cli
        in al, 0x70
        or al, 0x80
        out 0x70, al

        mov eax, cr0
        or eax, 1
        mov cr0, eax

        jmp 0x08:protected_mode

SECTION .data
    ; Macro for GDT entries
    %macro gdt_entry 4
        ; %1 - base addres
        ; %2 - limit
        ; %3 - access
        ; %4 - flags

        dw (%2 & 0xFFFF)
        dw (%1 & 0xFFFF)
        db ((%1 >> 16) & 0xFF)
        db (%3 & 0xFF)
        db ((%2 >> 16) & 0x0F | (%4 << 4))
        db ((%1 >> 24) & 0xFF)
    %endmacro

    gdt_description:
        dw (gdt_end - gdt_start - 1)                               ; size
        dd gdt_start                                               ; offset

    gdt_start:
        gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1100       ; code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1100       ; data segment
    gdt_end:

SECTION .bss

    ; Parameters to be passed to the bootloader
    GLOBAL bootloader_parameters
    bootloader_parameters:
        bootloader_parameters__boot_drive_id:        resb 1                  ; Boot drive ID
        bootloader_parameters__boot_parttition:      resb 1                  ; Boot partition
        bootloader_parameters__acpi_extended:        resb 1                  ; Memory map has ACPI extend attributes
        bootloader_parameters__memory_map_start:     resd 1                  ; Memory map start
        bootloader_parameters__memory_map_count:     resw 1                  ; Memory map number of entries

SECTION .magic
    dd 0x46554E42