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
        xor ax, ax
        mov ds, ax
        mov es, ax

        ; Setup stack
        mov ss, ax
        mov sp, 0x7A00
        mov bp, sp

        ; Relocate
        mov cx, 0x200
        mov si, 0x7C00
        mov di, 0x7A00
        rep movsb

        ; Jump
        jmp 0x00:relocated

SECTION .text
    EXTERN load_lba
    EXTERN print
    EXTERN error

    do_boot:
        mov bl, al
        add bl, '0'
        mov [do_boot__booting + 23], bl
        mov si, do_boot__booting
        call print

        dec al
        mov bl, 0x10
        mul bl

        mov si, pt_entry1
        add si, ax
        add si, 0x08

        mov bx, 0x7C00
        mov cl, 1
        call load_lba
        jc error

        ;
        ; Check if the sector is actually bootable
        ;
        cmp word [0x7C00 + 0x200 - 2], 0xAA55
        je jump_to_bootloader

        ; Check failed
        mov ah, 0xFD
        jmp error

    jump_to_bootloader:
        ; Loaded, let's clean up, it's not necessary but we are nice :)
        ; Clear general purpose registers
        xor ax, ax
        xor bx, bx
        xor cx, cx
        xor dh, dh       ; dl - drive boot number, preserve this
        xor di, di
        xor si, si

        ; Segment registers are already cleared, we never used them

        ; Fix stack
        mov sp, 0x7A00
        mov bp, sp

        ; Goodbye!
        jmp 0x00:0x7C00

    relocated:
        ; Hello world!
        mov si, relocated__welcome_message
        call print

        mov bx, pt_entry1   ; partition entry base in memory
        mov dh, 1           ; partition number starting from 1
        mov al, 0           ; active, bootable partition
        mov cx, 4

        relocated__loop:
            test byte [bx], 0x80
            jz relocated__loop_continue

            ; Found active

            ; If there already is an active, die
            cmp al, 0
            mov ah, 0xFE
            jne error

            mov al, dh

            relocated__loop_continue:
            inc dh
            add bx, 0x10
            loop relocated__loop

        cmp al, 0
        jne do_boot

        ; no bootable partitions
        mov ah, 0xFF
        jmp error

;
; Data
;
SECTION .rodata
    relocated__welcome_message:                 db "FunnyOS MBR", 0

SECTION .data
    do_boot__booting:                           db "Booting from partition ?", 0

SECTION .partition_table
    ; This will be overwritten by a partitioning tool, it is used only to have labels for the entries
    partition_table:
        pt_diskid: dd 0x12345678
        dw 0x00

        pt_entry1: times 0x10 db 0x00
        pt_entry2: times 0x10 db 0x00
        pt_entry3: times 0x10 db 0x00
        pt_entry4: times 0x10 db 0x00

    boot_signature:
        ; Boot signature
        dw 0xAA55