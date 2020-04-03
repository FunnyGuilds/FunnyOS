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

        print__loop:
           cmp byte [si], 0
           je print__ret

           mov al, [si]
           int 0x10
           inc si
           jmp print__loop

        print__ret:
           mov al, 0x0D
           int 0x10
           mov al, 0x0A
           int 0x10

           popa
           ret

    ; Prints the error code from AH and halts
    GLOBAL error
    error:
        xor ch, ch
        mov cl, ah
        shr cl, 4
        xor dh, dh
        mov dl, ah
        and dl, 0b00001111

        mov si, print_error__character_array
        add si, cx
        mov ax, [si]
        mov [print_error__buf + 0x07], al

        mov si, print_error__character_array
        add si, dx
        mov ax, [si]
        mov [print_error__buf + 0x08], al

        mov si, print_error__buf
        call print

    halt:
        cli
        hlt
        jmp halt

SECTION .rodata
    print_error__character_array:               db '0123456789ABCDEF'

SECTION .data
    print_error__buf:                           db "Error: ??h", 0