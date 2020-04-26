[bits 64]

SECTION .text
    main:
        ; Clear screen
        mov rdi, 0xB8000
        mov rcx, 80 * 25
        mov ah, 0
        mov al, ' '
        rep stosw

        ; Print
        mov rdi, 0xB8000
        lea rsi, [rel text]

        print_loop:
            lodsb
            mov [rdi], al
            mov [rdi + 1], byte 0x0F
            add rdi, 2
            test al, al
            jnz  print_loop

        halt:
            hlt
            jmp halt

    text:
        db "Hello world from 64-bit.", 0