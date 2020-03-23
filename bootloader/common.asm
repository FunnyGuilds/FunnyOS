;
; Prints a message from address in SI until it reaches a null byte.
;

print:
    pusha

    ; Clear DS
    xor dx, dx
    mov ds, dx

    ; Read active page number
    mov ah, 0x0F
    int 0x10

    ; BIOS Interrupt 0x0E - write character
    mov ah, 0x0E
    mov bh, 0

    print_loop:
    ; Use AL as a counter
    mov al, [ds:si]

    ; If its 0 we are finished
    test al, al
    jz print_return

    ; Otherwise print and loop back
    int 0x10
    inc si
    jmp print_loop

    print_return:
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10

    popa
    ret

setup_video_mode:
    push ax

    mov ah, 0x00
    mov al, 0x03                       ; 80x25 video mode
    int 0x10

    pop ax
    ret

; Writes 1 to ax if zero flag is set, otherwise writes 0
read_zf:
    lahf
    and ax, 0b0100000000000000
    shr ax, 14
    ret