; Sets ZF if A20 line is NOT enabled
check_a20_enabled:
    pusha
    push es
    push fs

    ; 0x500 - first address, the first byte that is guaranteed to be available
    mov ax, 0x0
    mov es, ax
    mov si, 0x500

    ; 0xFFFF:0x510 is 0x100500 - exactly 1 MiB after 0x500
    mov ax, 0xFFFF
    mov fs, ax
    mov di, 0x510

    mov [es:si], byte 0
    mov [fs:di], byte 1
    cmp [es:di], byte 1

    pop fs
    pop es
    popa
    ret

; Enable A20 line
enable_a20:
    call check_a20_enabled
    jnz a20_is_enabled

    a20_is_not_enabled:
        mov si, message_a20_enabling
        mov di, MSG_OK
        call log

         ; Try to use BIOS api
        call a20_enable_bios
        call check_a20_enabled
        jnz a20_is_enabled

        ; Failed to enable the A20 line
        mov si, message_a20_failed
        mov di, MSG_ERROR
        call log
        stc
        ret

    a20_is_enabled:
        mov si, message_a20_enabled
        mov di, MSG_OK
        call log
        clc
        ret

a20_enable_bios:
    pusha

    mov si, message_a20_bios_try
    mov di, MSG_OK
    call log

    ; Query for support
    mov ax, 0x2403
    int 0x15
    jc a20_enable_bios__unsupported

    ; Use API to query the state
    mov ax, 0x2402
    int 0x15
    cmp al, 1
    je a20_enable_bios__supposedly_enabled

    ; Actually enable the line
    mov ax, 0x2401
    int 0x15
    jc a20_enable_bios__failed

    ; Success!
    popa
    ret

    a20_enable_bios__unsupported:
        mov si, message_a20_bios_unsupported
        mov di, MSG_WARN
        call log
        popa
        ret

    a20_enable_bios__supposedly_enabled:
        mov si, message_a20_bios_already_enabled
        mov di, MSG_WARN
        call log
        popa
        ret

    a20_enable_bios__failed:
        cmp ah, 0x01
        je a20_enable_bios__failed_keyboard_secure

        mov si, message_a20_bios_failed
        mov di, MSG_WARN
        call log
        popa
        ret

    a20_enable_bios__failed_keyboard_secure:
        mov si, message_a20_bios_keyboard_secure
        mov di, MSG_WARN
        call log
        popa
        ret

message_a20_enabled:                    db "A20 line is enabled.", 0
message_a20_enabling:                   db "Enabling the A20 line.", 0
message_a20_failed:                     db "Failed to enable the A20 line.", 0
message_a20_bios_try:                   db "Trying to enable the A20 line using the BIOS api.", 0
message_a20_bios_already_enabled:       db "A20 line is supposedly enabled but the actual check failed [?]", 0
message_a20_bios_keyboard_secure:       db "... Failed, keyboard controller is in secure mode.", 0
message_a20_bios_failed:                db "... Failed", 0
message_a20_bios_unsupported:           db "... Unsupported", 0