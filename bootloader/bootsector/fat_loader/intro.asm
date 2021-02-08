%include "config.asm"

%define CONVENTIONAL_MEMORY_END 0x00080000
%define RESERVED_MEMORY 0x1000 * 3
%define STACK_TOP (CONVENTIONAL_MEMORY_END - RESERVED_MEMORY)

%define PML4_ADDRESS (CONVENTIONAL_MEMORY_END - 0x1000 * 3)
%define PDPE_ADDRESS (CONVENTIONAL_MEMORY_END - 0x1000 * 2)
%define PD_ADDRESS   (CONVENTIONAL_MEMORY_END - 0x1000 * 1)

EXTERN fat_loader
EXTERN load_lba

EXTERN fat_loader

[bits 16]
SECTION .intro

    intro:
        ; Fetch boot parametres
        mov     [boot_drive],     dl
        mov     [boot_partition], dh

        ; Setup video mode
        mov     ax, 0x0003
        int     0x10

        ; Disable interrupts
        cli
        in      al, 0x70
        or      al, 0x80
        out     0x70, al

        ; Load GDT and jump to protected mode
        lgdt    [gdt_description]

        mov     eax, cr0
        or      eax, 1
        mov     cr0, eax

        jmp     0x18:.protected

    .protected:
        [bits 32]
        mov     ax, 0x20
        mov     ds, ax
        mov     ss, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     esp, STACK_TOP

        ; Test for long mode
        mov     eax, 0x80000001
        cpuid
        test    edx, (1 << 29)
        jz      unsupported_machine

        ; Setup page tables
        xor     al, al              ; Clear PML4
        mov     ecx, 0x1000
        mov     edi, PML4_ADDRESS
        rep     stosb

        mov     ecx, 0x1000         ; Clear PDPE
        mov     edi, PDPE_ADDRESS
        rep     stosb

        mov     DWORD [PML4_ADDRESS], (PDPE_ADDRESS | 0b11)     ; Map first entry of PML4 to PDPE
        mov     DWORD [PDPE_ADDRESS], (PD_ADDRESS   | 0b11)     ; Map first entry of PDPE to Page Directory

        ; Map first 1 GB using 512 of 2 MB pages
        mov     eax, 0                  ; Current physical address
        mov     ecx, 0

    .map:
        mov     edx, eax                 ; Prepare entry in EDX
        or      edx, 0b10000011          ; Flags: present, writable, entry size

        ; Entries are 64-bit, we care only about low 32-bits
        mov     [PD_ADDRESS + ecx * 8 + 0], edx
        mov     [PD_ADDRESS + ecx * 8 + 4], DWORD 0

        add     eax, 0x1000             ; Next physical page
        inc     ecx
        cmp     ecx, 512
        jne     .map

        ; Setup long mode
        mov     eax, PML4_ADDRESS         ; Set PML4 address
        mov     cr3, eax

        mov     ecx, 0xC0000080           ; Set LME bit in EFER
        rdmsr
        or      eax, 1 << 8
        wrmsr

        mov     eax, cr4                  ; Enable PAE
        or      eax, 1 << 5
        mov     cr4, eax

        mov     eax, cr0                  ; Enable paging
        or      eax, 1 << 31
        mov     cr0, eax

        jmp     0x08:.long64

    .long64:
        [bits 64]
        mov     ax, 0x10
        mov     ds, ax
        mov     ss, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     rsp, STACK_TOP

        ; Jump to C code
        xor     rdi, rdi
        xor     rsi, rsi

        mov     dil, [boot_drive]
        mov     sil, [boot_partition]
        mov     rdx, STACK_TOP

        jmp     fat_loader


    unsupported_machine:
        [bits 32]
        ; TODO: Maybe print an error?
        cli
        hlt
        jmp $

SECTION .bss
    boot_drive:     resb 1
    boot_partition: resb 1

SECTION .data
    ; Macro for GDT entries
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
        dw (gdt_end - gdt_start - 1)                               ; size
        dd gdt_start                                               ; offset

    gdt_start:
        gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1010       ; 64-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1010       ; 64-bit data segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1100       ; 32-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1100       ; 32-bit data segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b1000       ; 16-bit code segment
        gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b1000       ; 16-bit data segment
    gdt_end:
