;
;   KekOS Stage1 bootloader
;
%include "data.asm"

[org BOOTLOADER_START_ADDRESS]
[bits 16]

;
; Starting code
;
start:
    ; Fuck the interrupts for now
    cli

    ; Get info about the booting disk from BIOS
    mov [data_boot_disk], dl

    ; Setup stack
    xor ax, ax
    mov ss, ax
    mov bp, 0x7C00
    mov sp, bp

    ; Clear segment register: ds
    xor si, si
    mov ds, si

    ; Setup video mode
    call setup_video_mode

    ; Print info and die
    mov si, message_info
    call print

    ; Read stage 2
    mov ah, 0x02
    mov al, STAGE2_SIZE_SECTORS       ; # of sectors to read
    mov ch, 0                         ; low bits of cylinder
    mov cl, 2                         ; sector number (bits 0-5) starting from 0, high bits of cyllinder (bits 3-7)
    mov dh, 0                         ; head number
                                      ; dl (drive number) already set by BIOS
    xor bx, bx
    mov es, bx
    mov bx, STAGE2_LOCATION           ; [es:bx] - destination
    int 0x13
    jc read_failed

    ; Check magic
    cmp dword [es:bx], STAGE2_MAGIC
    jnz magic_failed

    ; We gucci
    mov si, message_ok
    call print

    ; Jump to stage 2
    mov dh, 0
    mov dl, [data_boot_disk]
    push dx

    add bx, 0x04                         ; +4 to skip the magic
    jmp bx

    ; We should never reach here!
    mov si, message_stage2_returned
    call print
    jmp halt

read_failed:
    mov si, message_error_read
    call print
    jmp halt

magic_failed:
    mov si, message_magic_invalid
    call print
    jmp halt

;
; Includes
;
%include "common.asm"

;
; Data
;
data_boot_disk:             db 0 ; the number of the disk we are booting from

message_info:               db "KekOS Bootloader v0.0.1 - Starting ... ", 0
message_error_read:         db "ERROR: read failed ", 0
message_magic_invalid:      db "ERROR: verification failed ", 0
message_stage2_returned:    db "ERROR: stage2 returned ", 0
message_error_halt:         db "STAGE1 ERROR", 0
message_ok:                 db "Stage1 completed! Jumping to stage2.", 0

;
; If this code was reached, something went terribly wrong
;
halt:
    mov si, message_error_halt
    call print
    call do_halt

do_halt:
    cli
    hlt
    jmp do_halt


padding_and_magic:
    %if ($ - $$) > (MBR_BOOT_MAX_SIZE - 2)
        %fatal "stage1 code must not exceed 512 bytes"
    %endif

    times (MBR_BOOT_MAX_SIZE-2)-($-$$) db 0 ; padding
    db 0x55 ; legacy BIOS magic
    db 0xAA