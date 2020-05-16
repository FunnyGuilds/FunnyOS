[bits 16]

SECTION .text
    ;
    ; Utility function for printing a string in 16-bit real mode using BIOS interrupts.
    ; This adds a new line.
    ; Parameters:
    ;   SI - location of the null-terminated string.
    ;
    GLOBAL print
    print:
        pusha

        ; Fetch display mode
        mov ah, 0x0F
        int 0x10

        ; Setup interrupt
        mov ah, 0x0E
        mov bl, 0x00

        .loop:
           lodsb
           cmp al, 0
           je .ret

           int 0x10
           jmp .loop

        .ret:
           mov al, 0x0D
           int 0x10
           mov al, 0x0A
           int 0x10

           popa
           ret

    ; Prints the error code from AH and halts
    GLOBAL error
    error:
        movzx si, ah
        movzx di, ah
        shr si, 4           ; First digit
        and di, 0b00001111  ; Second digit

        mov cl, [print_error__character_array + si]
        mov [print_error__buf_digit1], cl

        mov cl, [print_error__character_array + di]
        mov [print_error__buf_digit2], cl

        mov si, print_error__buf_start
        call print

        .hang:
            cli
            hlt
            jmp .hang

SECTION .rodata
    print_error__character_array:               db '0123456789ABCDEF'

SECTION .data
    print_error__buf_start:                     db "Error: "
    print_error__buf_digit1:                    db "?"
    print_error__buf_digit2:                    db "?"
    print_error__buf_end:                       db "h", 0