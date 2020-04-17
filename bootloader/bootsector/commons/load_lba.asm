[bits 16]

SECTION .text
    ;
    ; Utility function for loading sectors from LBA addresses in real mode.
    ; Parameters:
    ;   SI - memory location at where the LBA address is at
    ;   DL - drive number
    ;   CL - number of sectors to load
    ;   ES:BX - memory location
    ;
    ; Output:
    ;   On success: CF clear
    ;   On fail: CF set, error code in AH
    ;
    GLOBAL load_lba
    load_lba:
        pusha
        xor ch, ch
        push bx
        push cx

        ; Read the boot sector
        mov [load_lba__drive], dl

        mov ah, 0x08                ; get drive parameters
        xor di, di
        int 0x13
        jc load_lba_fail

        ; Calculations
        and cl, 0b00111111
        mov ch, dh
        inc ch
        ; cl = sector per track
        ; ch - heads per cyllinder

        mov al, cl
        mul ch
        mov bx, ax
        call load_lba__fetch_address
        div bx
        mov [load_lba__cylinder], ax

        call load_lba__fetch_address
        xor bh, bh
        mov bl, cl
        div bx
        xor dx, dx
        div ch
        mov [load_lba__head], ah

        call load_lba__fetch_address
        mov bl, cl
        div bx
        inc dl
        mov [load_lba__sector], dl

        pop cx

        mov ah, 0x02
        mov al, cl
        mov ch, [load_lba__cylinder]
        mov cl, [load_lba__cylinder + 1]
        and cl, 0b11
        shl cl, 6
        mov dh, [load_lba__sector]
        and dh, 0b00111111
        or cl, dh
        mov dh, [load_lba__head]
        mov dl, [load_lba__drive]
        pop bx
        int 0x13
        jc load_lba_fail
        popa
        ret

        load_lba_fail:
        ; Skip registers pushed by pusha
        add sp, 16
        stc
        ret

        load_lba__fetch_address:
            mov al, [si + 0x00]
            mov ah, [si + 0x01]
            mov dl, [si + 0x02]
            mov dh, [si + 0x03]
            ret

SECTION .bss
    load_lba__drive:      resb 1
    load_lba__cylinder:   resw 1
    load_lba__head:       resb 1
    load_lba__sector:     resb 1