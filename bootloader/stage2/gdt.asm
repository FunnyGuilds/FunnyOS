setup_temporary_gdt:
    mov si, message_lgdt_start
    mov di, MSG_OK
    call log

    xor ax, ax
    mov ds, ax
    lgdt [gdt_description]

    mov si, message_lgdt_done
    mov di, MSG_OK
    call log

    ret

gdt_description:
    dw (gdt_end - gdt_start - 1)                               ; size
    dd gdt_start                                               ; offset

gdt_start:
    gdt_entry 0x00000000, 0x00000000, 0b00000000, 0b0000       ; null segment
    gdt_entry 0x00000000, 0x0FFFFFFF, 0b10011010, 0b0100       ; code segment
    gdt_entry 0x00000000, 0x0FFFFFFF, 0b10010010, 0b0100       ; data segment
gdt_end:

message_lgdt_start:                          db "Setting up temporary GDT.", 0
message_lgdt_done:                           db "Temporary GDT has been set up correctly.", 0

