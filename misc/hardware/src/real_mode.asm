[bits 32]
DEFAULT REL

SECTION .real
    GLOBAL g_gdtInfo        ; Symbol for C++
    g_gdtInfo:
        g_gdtInfo_selectorCode32: dw 0
        g_gdtInfo_selectorData32: dw 0
        g_gdtInfo_selectorCode16: dw 0
        g_gdtInfo_selectorData16: dw 0

    ; BIOS IDT
    g_biosIdt:
        dw (256 * 4) - 1
        dd 0x00000000

    ; Temporary IDT storage.
    g_storedIdt:
        dw 0x00
        dd 0x00

    ; Cache for 32-bit stack pointer.
    stackCache: dd 0x00

    GLOBAL g_savedRegisters:
    g_savedRegisters:
        saved_AX: dw 0x00
        saved_CX: dw 0x00
        saved_DX: dw 0x00
        saved_BX: dw 0x00
        saved_SI: dw 0x00
        saved_DI: dw 0x00
        saved_ES: dw 0x00
        saved_FS: dw 0x00
        saved_GS: dw 0x00
        saved_EFLAGS: dw 0x00

    ; Stack for 16-bit mode. Linker will put it somewhere low in memory.
    stack16:
        times (0x400 * 4) db 0x00       ; 4KB stack
    stack16_end:

    ; Buffer for real mode data
    GLOBAL g_realBuffer
    GLOBAL g_realBufferTop
    g_realBuffer:
        times 0x1000 db 0x00
    g_realBufferTop:

    GLOBAL do_real_mode_interrupt
    do_real_mode_interrupt:
        xchg bx, bx
        ; Save registers, prepare new stack
        push ebp

        ; Fetch section start into ebp
        call do_real_mode_interrupt_fetch_base_pointer
        do_real_mode_interrupt_fetch_base_pointer:
        pop ebp
        sub ebp, do_real_mode_interrupt_fetch_base_pointer - $$

        ; Setup new stack
        mov [ebp + (stackCache - $$)], esp
        xor esp, esp
        lea esp, [ebp + (stack16_end - $$)]

        ; Update int opcode
        mov eax, [ebp + (stackCache - $$)]
        mov eax, [eax + 4 * 2]
        mov [ebp + (interrupt_number - $$)], al

        ; Jump to 16-bit mode
        mov eax, [ebp + (g_gdtInfo_selectorData16 - $$)]
        mov ds, ax
        mov ss, ax

        push dword [ebp + (g_gdtInfo_selectorCode16 - $$)]
        lea eax, [ebp + (protected_16 - $$)]
        push dword eax
        retf

        protected_16:
        [bits 16]
        ; Now to real mode
        mov eax, cr0
        and eax, ~1
        mov cr0, eax

        push word 0
        lea ax, [ebp + (real_16 - $$)]
        push word ax
        retf

        real_16:
        xor ax, ax
        mov ds, ax
        mov ss, ax

        ; Replace IDT for the interrupt
        sidt [ebp + (g_storedIdt - $$)]
        lidt [ebp + (g_biosIdt - $$)]

        ; Set registers
        push word [ebp + (saved_EFLAGS - $$)]
        popf

        mov ax, [ebp + (saved_AX - $$)]
        mov cx, [ebp + (saved_CX - $$)]
        mov dx, [ebp + (saved_DX - $$)]
        mov bx, [ebp + (saved_BX - $$)]
        mov si, [ebp + (saved_SI - $$)]
        mov di, [ebp + (saved_DI - $$)]
        mov es, [ebp + (saved_ES - $$)]
        mov fs, [ebp + (saved_FS - $$)]
        mov gs, [ebp + (saved_GS - $$)]

        ; Execute interrupt
        interrupt_opcode: db 0xCD
        interrupt_number: db 0x00

        ; Read registers
        pushf
        pop word [ebp + (saved_EFLAGS - $$)]

        mov [ebp + (saved_AX - $$)], ax
        mov [ebp + (saved_CX - $$)], cx
        mov [ebp + (saved_DX - $$)], dx
        mov [ebp + (saved_BX - $$)], bx
        mov [ebp + (saved_SI - $$)], si
        mov [ebp + (saved_DI - $$)], di
        mov [ebp + (saved_ES - $$)], es
        mov [ebp + (saved_FS - $$)], fs
        mov [ebp + (saved_GS - $$)], gs

        ; Restore previous IDT
        lidt [ebp + (g_storedIdt - $$)]

        ; Back to protected
        mov eax, cr0
        or eax, 1
        mov cr0, eax

        push word [ebp + (g_gdtInfo_selectorCode32 - $$)]
        lea ax, [ebp + (protected_32 - $$)]
        push word ax
        retf

        protected_32:
        [bits 32]
        mov eax, [ebp + (g_gdtInfo_selectorData32 - $$)]
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov esp, [ebp + (stackCache - $$)]
        pop ebp
        ret