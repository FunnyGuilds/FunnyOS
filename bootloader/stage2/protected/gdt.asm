; GDT table for long mode
p_setup_temporary_gdt:
    mov esi, p_message_lgdt_start
    call p_print

    xor ax, ax
    mov ds, ax
    lgdt [p_gdt_description]

    mov esi, p_message_lgdt_start
    call p_print

    ret

p_gdt_description:
    dw (p_gdt_end - p_gdt_start - 1)                               ; size
    dd p_gdt_start                                               ; offset

p_gdt_start:
    gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
    gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b0100       ; code segment
    gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b0100       ; data segment
p_gdt_end:

p_message_lgdt_start:                          db "Setting up temporary 64-bit GDT.", 0
p_message_lgdt_done:                           db "Temporary 64-bit GDT has been set up correctly.", 0

