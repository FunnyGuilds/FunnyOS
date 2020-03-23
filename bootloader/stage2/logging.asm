%define MSG_OK     0x00
%define MSG_WARN   0x02
%define MSG_ERROR  0x04

;
; Prints error code from AH
;

print_error_code:
    pusha
    push ds

    ; Zero DX
    xor bx, bx
    mov ds, bx

    ; Get first character
    mov al, ah
    shr al, 4
    mov bx, print_error_code__characters
    add bl, al
    mov ch, [ds:bx]

    ; Get second character
    and ah, 0x0F
    mov bx, print_error_code__characters
    add bl, ah
    mov cl, [ds:bx]

    ; Setup message
    mov bx, print_error_code__message
    add bx, 12
    mov [ds:bx], ch
    inc bx
    mov [ds:bx], cl

    ; Log
    mov si, print_error_code__message
    mov di, MSG_ERROR
    call log

    pop ds
    popa
    ret

    print_error_code__characters:       db "0123456789ABCDEF"
    print_error_code__message:          db "Error code: ??h", 0

log:
    pusha

    mov cx, di

    ; Clear DS
    xor ax, ax
    mov ds, ax

    ; Read active page number
    mov ah, 0x0F
    int 0x10

    ; BIOS Interrupt 0x0E - write character
    mov ah, 0x0E
    mov bl, 0

    ; prologue
    mov di, log__msg_pre
    call log__print_fast

    ; severity
    mov di, cx
    add di, log__msg_switch_table
    mov di, [ds:di]
    call log__print_fast

    ; message
    mov di, si
    call log__print_fast

    ; epilogue
    mov di, log__msg_ep
    call log__print_fast

    popa
    ret

    log__print_fast:
        mov al, [ds:di]
        int 0x10
        inc di
        cmp byte [ds:di], 0
        jnz log__print_fast
        ret

    log__msg_pre:   db " * ", 0
    log__msg_ep:    db 0x0D, 0x0A, 0
    log__msg_ok:    db "[ OK ] ", 0
    log__msg_warn:  db "[WARN] ", 0
    log__msg_error: db "[ERR ] ", 0

    log__msg_switch_table:
        dw log__msg_ok
        dw log__msg_warn
        dw log__msg_error