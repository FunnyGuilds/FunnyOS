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

%macro gdt_entry 4
    ; %1 - base addres
    ; %2 - limit
    ; %3 - access
    ; %4 - flags

    dw (%2 & 0xFFFF)
    dw (%1 & 0xFFFF)
    db ((%1 >> 16) & 0xFF)
    db (%3 & 0xFF)
    db ((%2 >> 16) & 0x0F | (%4 << 4))
    db ((%1 >> 24) & 0xFF)
%endmacro

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

