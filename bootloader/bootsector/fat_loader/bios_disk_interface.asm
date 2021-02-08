%include "config.asm"

EXTERN load_lba

SECTION .text
    GLOBAL fl_load_from_disk
    fl_load_from_disk:
        [bits 64]
        ; Save state
        push    rbp
        mov     rbp, rsp
        push    rbx
        mov     [cached_rsp], rsp

        ; Jump to compat 16-bit mode
        push    0x30                  ; ss
        push    QWORD rm_stack_top    ; sp
        pushf                         ; flags
        push    0x28                  ; cs
        push    .compat16             ; ip
        iretq

    .compat16:
        [bits 16]
        mov     edx, cr0
        and     edx, ~((1 << 0) | (1 << 31))
        mov     cr0, edx
        jmp     0x00:.real16

    .real16:
        xor     ax, ax
        mov     ds, ax
        mov     ss, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax

        ; Call bios
        mov     eax, esi
        mov     dx, di
        mov     cl, 1
        mov     bx, g_read_buffer
        call    load_lba
        jc      .return_to_long

        ; No carry, success, set the return code to 0
        mov     ah, BYTE 0

    .return_to_long:
        mov     edx, cr0                      ; Set protected bit and paging bit in cr0
        or      edx, ((1 << 0) | (1 << 31))
        mov     cr0, edx

        jmp     0x08:.long

    .long:
        [bits 64]
        mov     dx, 0x10
        mov     ds, dx
        mov     ss, dx
        mov     es, dx
        mov     fs, dx
        mov     gs, dx

        ; Restore state
        mov     rsp, [cached_rsp]
        pop     rbx
        mov     rsp, rbp
        pop     rbp

        ; Zero-extend result
        mov     al, ah
        movzx   rax, al
        ret

SECTION .bss
    cached_rsp: resq 1

SECTION .rm_data
    GLOBAL g_read_buffer
    g_read_buffer: times 0x200 db 0

    rm_stack:
        times 0x400 db 0x00

    rm_stack_top:
