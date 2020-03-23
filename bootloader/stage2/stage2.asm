%include "data.asm"

[org STAGE2_LOCATION]
[bits 16]

magic:
    dd STAGE2_MAGIC
    jmp start

;
; Includes
;
%include "common.asm"
%include "stage2/logging.asm"
%include "stage2/a20line.asm"
%include "stage2/gdt.asm"
%include "stage2/drive.asm"

;
; Stage2 main
;
start:
    ; Pop arguments from stage1
    pop dx
    mov [data_boot_disk], dh

    ; Setup video mode again
    call setup_video_mode

    ; Print header
    mov si, message_header
    call print

    ; Some logging
    mov si, message_info
    mov di, MSG_OK
    call log

    ; Initialization
    call enable_a20
    jc fail

    ; Load kernel
    mov dl, [data_boot_disk]
    mov bx, KLD_STARTING_SECTOR
    mov cx, KLD_SECTORS
    mov di, (KLD_ADDRESS / 0x10)
    mov fs, di
    xor di, di
    call load_data
    jc fail

    ; Kernel is loaded
    mov si, message_loading_kernel_done
    mov di, MSG_OK
    call log

    ; Setup GDT
    call setup_temporary_gdt

    ; Disable interrupts and NMI
    cli
    in al, 0x70
    and al, 0b01111111
    out 0x70, al

    ; Jump to protected mode
    mov si, message_protected_mode
    mov di, MSG_OK
    call log

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

; Protected mode code
%include "stage2/protected/protected.asm"

[bits 16]
; Something went horribly wrong if we are here.
fail:
    mov si, message_fail
    mov di, MSG_ERROR
    call log

    mov si, message_goodbye
    call print

    halt:
        cli
        hlt
        jmp halt

;
; Data
;

data_boot_disk:             db 0            ; the number of the disk we are booting from

message_header:
db "+==============================================================================+"
db "|                                                                              |"
db "|                       KekOS Bootloader. Version 0.0.1                        |"
db "|                                                                              |"
db "+==============================================================================+"
db 0


message_info:                           db "Bootloader initialized.", 0
message_loading_kernel:                 db "Loading kernel to memory!", 0
message_loading_kernel_done:            db "Done!", 0
message_protected_mode                  db "Enabling protected mode...", 0
message_fail:                           db "Boot failed.", 0
message_goodbye:                        db 0x0D, 0x0A, "Goodbye", 0


padding:
    %if ($ - $$) > (STAGE2_SIZE_SECTORS * 512)
        %fatal "stage2 code must not exceed 62 sectors"
    %endif

    times (STAGE2_SIZE_SECTORS * 512)-($-$$) db 0 ; padding

    db 0x69
    times (KLD_SIZE-2) db 0xcc ; TODO temp
    db 0x42