[bits 32]

%include "stage2/protected/screen.asm"
%include "stage2/protected/cpuid.asm"
%include "stage2/protected/paging.asm"
%include "stage2/protected/gdt.asm"

protected_mode:
    ; Setup segments
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x7C00
    mov ebp, esp

    ; Disable cursor and clear the screen
    mov dx, 0x3D4
    mov al, 0x0A
    out dx, al
    mov dx, 0x3D5
    mov al, 0b00100000
    out dx, al

    call p_clear_screen
    mov esi, p_message_entered
    call p_print

    ; Testing for CPUID
    call p_test_for_cpuid
    jc p_fail

    call p_print_cpuid_info

    ; Testing for long mode availability
    call p_check_long_mode_supported
    jc p_fail

    ; Setup paging
    mov esi, p_message_paging_setup
    call p_print

    ; Setup GDT

    jmp $

; Final, long mode code
%include "stage2/protected/long.asm"

[bits 32]
p_fail:
    mov esi, p_message_fail
    call p_print

    p_hang:
        cli
        hlt
        jmp p_hang

p_message_entered:              db "Entered protected mode!", 0
p_message_paging_setup:         db "Setting up paging!", 0
p_message_fail:                 db "Booting failed!", 0
