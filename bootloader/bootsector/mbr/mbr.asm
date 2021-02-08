;
; Stage1 Bootloader code
;
[bits 16]
;
; The intro section should use no addressing as the linker thinks that the code is at another location at this point.
;
SECTION .intro
    intro:
        ; We don't need any interrupts at this stage
        cli

        ; Clear segment registers
        xor     ax, ax
        mov     ds, ax
        mov     es, ax

        ; Setup stack
        mov     ss, ax
        mov     sp, 0x7A00
        mov     bp, sp

        ; Relocate
        mov     cx, 0x200
        mov     si, 0x7C00
        mov     di, 0x7A00
        rep     movsb

        ; Jump
        jmp     0x00:relocated

SECTION .text
    EXTERN load_lba
    EXTERN print
    EXTERN error

    relocated:
        ; Hello world!
        mov     si, welcome_message
        call    print

        mov     al, 1                      ; Partition number
        mov     bx, partition_table.entry1 ; Partition entry base
        mov     cx, 4                      ; Partition entries count

    .loop:
        call    try_boot
        inc     al
        add     bx, 0x10
        loop    .loop

        ; Booting failed, no bootable partitions
        mov     ah, 0xFF
        jmp     error

    try_boot:
        ; Check if active
        test    BYTE [bx], 0x80
        jnz     boot
        ret

    boot:
        ; AL - partition number
        ; BX - partition entry base
        mov     cl, al
        add     cl, '0'
        mov     [booting + 23], cl
        mov     si, booting
        call    print

        ; Read VBR
        mov     eax, [bx + 0x08]
        mov     bx, 0x7C00
        mov     cl, 1
        call    load_lba
        jc      error

        ; Check if the sector is actually bootable
        cmp     WORD [0x7C00 + 0x200 - 2], 0xAA55
        je      .jump_to_vbr

        ; Check failed
        mov ah, 0xFD
        jmp error

    .jump_to_vbr:
        jmp 0x00:0x7C00

;
; Data
;
SECTION .rodata
    welcome_message: db "FunnyOS MBR", 0

SECTION .data
    booting:         db "Booting from partition ?", 0

SECTION .partition_table
    ; This will be overwritten by a partitioning tool, it is used only to have labels for the entries
    partition_table:
        .diskid: dd 0x12345678
        dw 0x00

        .entry1: times 0x10 db 0x00
        .entry2: times 0x10 db 0x00
        .entry3: times 0x10 db 0x00
        .entry4: times 0x10 db 0x00

    boot_signature:
        ; Boot signature
        dw 0xAA55
