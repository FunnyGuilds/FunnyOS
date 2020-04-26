[bits 32]

%define PAGE_SIZE 0x1000

%macro BOOT_ERROR 1
    mov eax, 0
    int 0x2F
%endmacro

%macro PRINT 1
    mov eax, 1
    lea esi, [ebp + (%%message - $$)]
    int 0x2F
    jmp %%code

    %%message: db %1, 0
    %%code:
%endmacro


EXTERN TEXT_OFFSET
EXTERN PAGE_OFFSET
EXTERN SIZE

SECTION .entry
    entry:
        ; Fetch EIP into EBP
        call entry_fetch_address
        entry_fetch_address:
        pop ebp

        ; Fetch the beginning of .entry section
        sub ebp, (entry_fetch_address - $$)
        mov eax, ebp

        ; Make sure we are page aligned
        mov eax, ebp
        mov ebx, PAGE_SIZE
        mov edx, 0
        div ebx
        cmp edx, 0
        je entry_jump_to_main

        ; Not page aligned
        BOOT_ERROR 1

        entry_jump_to_main:
        add ebp, TEXT_OFFSET
        ; EBP = .text section
        jmp ebp

SECTION .text
    main:
        PRINT "main()"

        ; Setup long mode
        mov ecx, 0xC0000080
        rdmsr
        or eax, 1 << 8
        wrmsr

        ; Enable PAE
        mov eax, cr4
        or eax, 1 << 5
        mov cr4, eax

        ; Set pml4 address
        pop eax
        mov cr3, eax

        ; Enable paging
        mov eax, cr0
        or eax, 1 << 31
        mov cr0, eax

        ; Setup new gdt
        lea eax, [ebp + (gdt_start - $$)]
        push dword eax
        lea eax, [gdt_end - gdt_start - 1]
        push word ax
        lgdt [esp]
        add esp, 6

        ; Switch to 64-bit mode
        push dword 0x18
        lea eax, [ebp + (main_64 - $$)]
        push dword eax
        retf

    [bits 64]
    main_64:
        ; Setup 64-bit data segments
        mov rax, 0x20
        mov ds, rax
        mov ss, rax
        mov es, rax
        mov fs, rax
        mov gs, rax

        ; Jump to kernel
        pop rbx
        jmp rbx

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

    gdt_start:
        gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1100       ; 32-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1100       ; 32-bit data segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1010       ; 64-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1010       ; 64-bit data segment
    gdt_end: