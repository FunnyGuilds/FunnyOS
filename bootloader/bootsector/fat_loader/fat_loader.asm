%define BOOTLOADER_MAGIC                0x46554E42

%macro push_addr_16 1
    %if __BITS__ == 16
        push %1
    %elif __BITS__ == 32
        sub esp, 2
        mov [esp], word %1
    %endif
%endmacro

%macro push_addr_32 1
    %if __BITS__ == 16
        push 0
        push %1
    %elif __BITS__ == 32
        push %1
    %endif
%endmacro

[bits 16]
SECTION .intro
    EXTERN fat_loader
    EXTERN load_lba

    intro:
        mov [g_boot_drive], dl
        mov [g_boot_partition], ch

        mov esp, stack_end

        lgdt [gdt_description]

        push_addr_32 fat_loader
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
        jmp 0x08:jump_to_protected_far
        jump_to_protected_far:
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
    jump_to_real:
        ; First jump to protected 16-bit mode
        jmp 0x18:jump_to_real_protected_16
        jump_to_real_protected_16:
        [bits 16]
        push eax
        mov eax, cr0
        and eax, ~1
        mov cr0, eax
        jmp 0x00:jump_to_real_far
        jump_to_real_far:
        xor ax, ax
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        pop eax
        ret

    [bits 32]
    GLOBAL fl_print
    fl_print:
        push_addr_16 fl_print_16
        jmp jump_to_real
        fl_print_16:
        [bits 16]
        pushad
        mov esi, [esp + 9 * 4]
        mov ah, 0x0F
        int 0x10
        mov ah, 0x0E
        mov bl, 0x00

        cld
        fl_print__loop:
           lodsb
           cmp al, 0
           je fl_print__ret
           int 0x10
           jmp fl_print__loop

        fl_print__ret:
        popad
        ; 32-bit return address already on the stack
        jmp jump_to_protected

    [bits 32]
    GLOBAL fl_load_from_disk
    fl_load_from_disk:
        ; Deal with the parameters first
        push ebp
        mov ebp, esp
        pushad

        ; LBA
        mov eax, [ebp + 4 * 2]
        mov [load_from_disk__lba], eax

        ; Buffer**
        mov esi, [ebp + 4 * 3]
        mov [esi], dword load_from_disk__buffer

        push_addr_16 fl_load_from_disk_16
        jmp jump_to_real
        fl_load_from_disk_16:
        [bits 16]

        mov si, load_from_disk__lba
        mov dl, [g_boot_drive]
        mov cl, 1
        xor bx, bx
        mov es, bx
        mov bx, load_from_disk__buffer

        call load_lba
        popad
        pop ebp

        xor eax, eax
        pushf
        pop ax
        jnc fl_load_from_ok

        ; Fail, move ah to al
        shr ax, 8
        and eax, 0xFFFF
        jmp jump_to_protected

        fl_load_from_ok:
            xor eax, eax
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
        and eax, 0xFFFF

        ; AX now holds the address to jump to
        push_addr_16 fl_jump_to_bootloader_real
        jmp jump_to_real
        fl_jump_to_bootloader_real:
        [bits 16]
        mov dl, [g_boot_drive]
        mov ch, [g_boot_partition]
        jmp ax

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
    GLOBAL g_boot_drive
    GLOBAL g_boot_partition
    g_boot_drive:                         resb 1
    g_boot_partition:                     resb 1

    stack:                                resb 0x200
    stack_end:

    load_from_disk__lba:                  resd 1
    load_from_disk__buffer:               resd 0x200

SECTION .magic
    dd 0x46554E42