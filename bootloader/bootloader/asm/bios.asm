[bits 64]

%define STACK_CANARY 0xDE1C759F

SECTION .bios_text
    GLOBAL do_call_bios
    do_call_bios:
        ; Save non-scratch SystemV ABI registers
        push    rbx
        push    rbp
        push    r12
        push    r13
        push    r14
        push    r15

        ; Replace interrupt call number
        mov     al, [g_biosState.interrupt_number]
        mov     [.interrupt_number], al

        ; Set stack canary
        mov     eax, STACK_CANARY
        mov     [g_biosState.stack_canary], eax

        ; Cache RSP & drop to 16-bit compat mode
        mov     [g_biosState.cached_rsp], rsp

        xor     rax, rax
        xor     rcx, rcx
        mov     ax, [g_biosState.selector_16_data]
        mov     cx, [g_biosState.selector_16_code]

        push    QWORD rax                                        ; ss
        push    QWORD g_biosState.stack_top                      ; sp
        pushfq                                                   ; flags
        push    QWORD rcx                                        ; cs
        push    QWORD .long_to_compat                            ; rsp
        iretq

        [bits 16]
        .long_to_compat:
        mov     eax, cr0
        and     eax, ~((1 << 0) | (1 << 31))                     ; Clear protected mode and paging flag
        mov     cr0, eax
        jmp     0x00:.compat_to_real

        .compat_to_real:
        ; Reset data segments
        xor     ax, ax
        mov     ds, ax
        mov     es, ax
        mov     ss, ax

        ; Save GDT & IDT
        sgdt    [g_biosState.cached_gdt]
        sidt    [g_biosState.cached_idt]

        ; Prepare register values
        mov     eax,  [g_biosState.EAX]
        mov     ecx,  [g_biosState.ECX]
        mov     edx,  [g_biosState.EDX]
        mov     ebx,  [g_biosState.EBX]
        mov     edi,  [g_biosState.EDI]
        mov     esi,  [g_biosState.ESI]
        mov     ebp,  [g_biosState.EBP]
        mov     es,   [g_biosState.ES]
        mov     fs,   [g_biosState.FS]
        mov     gs,   [g_biosState.GS]

        push    DWORD [g_biosState.EFLAGS]
        popf

        jmp 0x00:.test
        .test:

        ; Self-modifying code: interrupt call 0xCD ib (ib - interrupt vector)
        db      0xCD
        .interrupt_number:
        db      0x00

        ; Save register values
        pushfd
        pop     DWORD [g_biosState.EFLAGS]

        mov     [g_biosState.EAX], eax
        mov     [g_biosState.ECX], ecx
        mov     [g_biosState.EDX], edx
        mov     [g_biosState.EBX], ebx
        mov     [g_biosState.EDI], edi
        mov     [g_biosState.ESI], esi
        mov     [g_biosState.EBP], ebp
        mov     [g_biosState.ES],  es
        mov     [g_biosState.FS],  fs
        mov     [g_biosState.GS],  gs

        ; Restore GDT & LDT
        lgdt    [g_biosState.cached_gdt]
        lidt    [g_biosState.cached_idt]

        ; Check stack canary
        mov     eax, STACK_CANARY
        cmp     [g_biosState.stack_canary], eax
        jne     .stack_canary_check_failed

        ; Back to long mode
        mov     eax, cr0
        or      eax, ((1 << 0) | (1 << 31))     ; Set protected mode and paging bit
        mov     cr0, eax

        cli
        push    WORD [g_biosState.selector_64_code]
        push    WORD .real_to_long
        iret

        [bits 64]
        .real_to_long:
        ; Restore data selectors
        mov     ax, [g_biosState.selector_16_data]
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax

        ; Restore RSP and non-scratch
        mov     rsp, [g_biosState.cached_rsp]

        ; Restore data segments
        .ret:
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     rbp
        pop     rbx
        ret

        .stack_canary_check_failed:
        cli
        jmp $

SECTION .bios_data
    GLOBAL g_biosState
    g_biosState:
        .EAX:     dd 0
        .ECX:     dd 0
        .EDX:     dd 0
        .EBX:     dd 0
        .EDI:     dd 0
        .ESI:     dd 0
        .EBP:     dd 0
        .EFLAGS:  dd 0
        .ES:      dd 0
        .FS:      dd 0
        .GS:      dd 0

        .selector_64_code: dw 0
        .selector_64_data: dw 0
        .selector_16_code: dw 0
        .selector_16_data: dw 0

        .interrupt_number: db 0

        .cached_gdt: times 10 db 0
        .cached_idt: times 10 db 0

        .cached_rsp:   dq 0
        .stack_canary: dd 0
        .stack:        times 0x1000 db 0
        .stack_top:
