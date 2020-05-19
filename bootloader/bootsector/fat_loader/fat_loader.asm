%include "config.asm"

[bits 16]
SECTION .intro
    EXTERN fat_loader
    EXTERN load_lba

    intro:
        ; Fetch boot parametres
        mov [g_boot_drive],     dl
        mov [g_boot_partition], dh

        ; Setup stack
        mov esp, stack_end

        ; Setup video mode
        mov ax, 0x0003
        int 0x10

        ; Disable interrupts
        cli
        in al, 0x70
        or al, 0x80
        out 0x70, al

        ; Jump to protected
        lgdt [gdt_description]
        push dword fat_loader ; C code entry
        jmp jump_to_protected

SECTION .text
    [bits 16]
    ;
    ; Jumps to protected mode and returns, expects 32-bit return address on the stack
    ;
    jump_to_protected:
        push eax
        mov eax, cr0
        or eax, 1
        mov cr0, eax
        jmp 0x08:.protected32
        .protected32:
        [bits 32]
        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        pop eax
        ret

    ;
    ; Jumps to real mode and returns, expects 16-bit return address on the stack
    ;
    [bits 32]
    jump_to_real:
        ; First jump to protected 16-bit mode
        jmp 0x18:.protected16
        .protected16:
        [bits 16]
        push eax
        mov eax, cr0
        and eax, ~1
        mov cr0, eax
        jmp 0x00:.real16
        .real16:
        xor ax, ax
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        pop eax
        ret

    [bits 32]
    EXTERN g_read_buffer
    GLOBAL fl_load_from_disk
    fl_load_from_disk:
        ; Store state
        push ebp
        mov ebp, esp
        pushad

        ; LBA
        mov eax, [ebp + 4 * 2]

        ; Jump to real mode
        push word .real
        jmp jump_to_real

        [bits 16]
        .real:
            mov dl, [g_boot_drive]
            mov cl, 1
            xor bx, bx
            mov es, bx
            mov bx, g_read_buffer
            call load_lba
            jc .fail

            ; Success
            mov [load_from_disk__return], dword 0
            jmp .ret

        .fail:
            movzx eax, ah
            mov [load_from_disk__return], eax

        .ret:
            popad
            mov esp, ebp
            pop ebp

            mov eax, [load_from_disk__return]

            ; Return via jump_to_protected, return address is on top of the stack
            jmp jump_to_protected

    [bits 32]
    GLOBAL fl_hang
    fl_hang:
        cli
        hlt
        jmp fl_hang

    [bits 32]
    GLOBAL fl_jump_to_bootloader
    fl_jump_to_bootloader:
        add esp, 4 ; We don't need return address since we are never returning
        pop eax

        ; AX now holds the address to jump to
        mov dl, [g_boot_drive]
        mov dh, [g_boot_partition]

        ; Jump to bootloader
        push word ax
        jmp jump_to_real

SECTION .data
    %macro gdt_entry 4
        ; %1 - base address
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
        dw (gdt_end - gdt_start - 1)                               ; size - 1
        dd gdt_start                                               ; offset

    gdt_start:
        gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1100       ; code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1100       ; data segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1000       ; 16-bit code segment
    gdt_end:


SECTION .bss
    GLOBAL g_boot_partition
    g_boot_drive:                         resb 1
    g_boot_partition:                     resb 1

    stack:                                resb 0x800
    stack_end:

    load_from_disk__buffer:               resd 0x200
    load_from_disk__return:               resd 1