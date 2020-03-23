;
; Simple screen driver for VGA mode 0x03
;
p_clear_screen:
    push ecx
    push edi

    mov ecx, 80*25
    mov edi, 0

    p_clear_screen__loop:
        mov [0xB8000+edi], word 0x0F20
        add edi, 2
        loop p_clear_screen__loop

    pop edi
    pop ecx
    ret


p_data_cursor: dd 0

p_scroll_line:
    pushad

    mov ebx, 1

    p_scroll_line__loop:
        mov esi, 0xB8000
        mov eax, 80*2
        mul ebx
        add esi, eax

        mov edi, esi
        sub edi, 80*2

        mov ecx, 80
        p_scroll_line__line_loop:
            movsw
            loop p_scroll_line__line_loop

        inc ebx
        cmp ebx, 25
        jne p_scroll_line__loop

    ; Clear last line
    mov esi, 0xB8000 + 24*80*2

    mov ecx, 0x80
    p_scroll_line__clear_loop:
        mov [esi], dword 0x0F20
        add esi, 2
        loop p_scroll_line__clear_loop

    mov [p_data_cursor], dword 80*24

    popad
    ret

p_print:
    pushad
    mov edi, [p_data_cursor]

    ; Scrolling
    p_print__loop:
        cmp byte [esi], 0
        je p_print__done

        cmp edi, 80*25
        jl p_print__loop_continue

        call p_scroll_line
        mov edi, [p_data_cursor]

        p_print__loop_continue:

        mov dl, [esi]
        mov [0xB8000 + edi * 0x02 + 0x00], dl
        mov [0xB8000 + edi * 0x02 + 0x01], byte 0x0F

        inc esi
        inc edi
        jmp p_print__loop

    p_print__done:

    ; Padding
    xor edx, edx
    mov eax, edi
    mov ecx, 80
    div ecx
    sub ecx, edx

    cmp ecx, 80
    je p_print__ret

    p_print__padding_loop:
        mov [0xB8000 + edi * 0x02 + 0x00], word 0x0F20
        inc edi
        loop p_print__padding_loop

    p_print__ret:

    mov [p_data_cursor], edi
    popad
    ret