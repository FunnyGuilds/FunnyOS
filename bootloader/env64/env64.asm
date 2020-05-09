[bits 32]

%define PAGE_SIZE 0x1000

EXTERN TEXT_OFFSET
EXTERN PAGE_OFFSET
EXTERN SIZE

SECTION .text
    main:
        ; Fetch EIP into EBP
        call main_fetch_address
        main_fetch_address:
        pop ebp

        ; Fetch the beginning of .text section
        sub ebp, (main_fetch_address - $$)

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

    gdt_start:
        gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1100       ; 32-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1100       ; 32-bit data segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1010       ; 64-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1010       ; 64-bit data segment
    gdt_end: