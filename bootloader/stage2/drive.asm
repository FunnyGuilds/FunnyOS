;
; Loads drive geometry for drive number DL
;
; Outputs:
; AX - number of heads in a track
; BL - number of sectors in a head
; CX - number of sectors in a track
;
; Clears CF on success, sets CF on fail.
; If this routine fails the values in the output registers are unspecified.
;
fetch_drive_geometry:
    push bp
    mov bp, sp
    sub sp, 5

    pusha
    push es

    mov ah, 0x08
    xor di, di
    mov es, di
    int 0x13
    jc fetch_drive_geometry__failed

    inc dh
    mov [bp-0x05], dh               ; # of heads in a track

    and cl, 0b00111111
    mov [bp-0x03], cl               ; # of sectors in a head

    ; cl * dh
    mov al, dh
    mul cl
    mov [bp-0x02], ax               ; # of sectors in a track

    ; Done
    clc

    fetch_drive_geometry__return:
        pop es
        popa

        mov ax, [bp-0x05]
        mov bl, [bp-0x03]
        mov cx, [bp-0x02]

        mov sp, bp
        pop bp

        ret

    fetch_drive_geometry__failed:
        stc
        jmp fetch_drive_geometry__return

;
; Calculates the head, sector and track id based on the given sector ID.
; Inputs
; AX - number of heads in a track
; BL - number of sectors in a head
; CX - number of sectors in a track
; DX - the sector number to calculate
; Outputs:
; AX - the track number
; BL - the head number
; CL - the sector number
;
calculate_chs:
    push bp
    mov bp, sp
    sub sp, 7

    pusha
    dec dx

    mov [bp-0x07], ax
    mov [bp-0x05], bl
    mov [bp-0x04], cx
    mov [bp-0x02], dx

    mov ax, dx
    xor dx, dx
    div word [bp-0x04]
    mov [bp-0x04], ax

    mov ax, dx
    div byte [bp-0x05]
    inc ah
    mov [bp-0x02], al
    mov [bp-0x01], ah

    popa

    mov ax, [bp-0x04]
    mov bl, [bp-0x02]
    mov cl, [bp-0x01]

    mov sp, bp
    pop bp

    ret


;
; Loads CX sectors of data, starting from BX from disk DL to memory at [FS:DI]
;
load_data:
    push bp
    mov bp, sp
    sub sp, 7

    pusha
    push ds
    push es
    push fs

    ; Fetch drive geometry
    pusha
    mov [bp-0x06], dl
    call fetch_drive_geometry
    mov [bp-0x02], ax
    mov [bp-0x03], bl
    mov [bp-0x05], cx
    popa
    jc load_data__fail

    mov ax, 0                ; current sector

    load_data__loop:
        pusha
        mov dx, bx
        add dx, ax
        mov ax, [bp-0x02]
        mov bl, [bp-0x03]
        mov cx, [bp-0x05]
        call calculate_chs

        mov dl, [bp-0x06]
        mov dh, bl
        mov ch, al
        mov ah, 0x02
        mov al, 1

        xor bx, bx
        mov es, bx
        mov bx, load_data__buf

        int 0x13
        jnc load_data__loop_success

        ; Fail
        call print_error_code
        popa
        jmp load_data__fail

        load_data__loop_success:
        popa

        ; Copy data
        pusha

        xor cx, cx
        mov ds, cx
        mov cx, fs
        mov es, cx

        ; Setup SI, DI is already set
        mov si, load_data__buf

        ; Setup counter
        mov cx, (0x200 / 0x04)

        load_data__copy_loop:
            movsd
            loop load_data__copy_loop

        popa

        ; Advance memory address
        mov si, fs
        add si, 0x20
        mov fs, si

        ; Increment current sector
        inc ax
        loop load_data__loop

    ; loop exited, we are done
    clc

    load_data__return:
        pop fs
        pop es
        pop ds
        popa

        mov sp, bp
        pop bp
        ret

    load_data__fail:
        stc
        jmp load_data__return

    load_data__buf:
        times 0x200 db 0