[bits 32]

SECTION .real.data
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

SECTION .real.text
    GLOBAL do_real_mode_interrupt
    do_real_mode_interrupt:
        ; Save registers, prepare new stack
        push ebp
        mov [stackCache], esp

        ; Setup new stack
        xor esp, esp
        xor ebp, ebp
        mov sp, stack16_end
        mov bp, sp

        mov eax, [stackCache]       ; Update int opcode
        mov eax, [eax + 4 * 2]
        mov [interrupt_number], al

        ; Jump to 16-bit mode
        mov eax, [g_gdtInfo_selectorData16]
        mov ds, ax
        mov ss, ax

        push dword [g_gdtInfo_selectorCode16]
        push dword protected_16
        retf

        protected_16:
        [bits 16]
        ; Now to real mode
        mov eax, cr0
        and eax, ~1
        mov cr0, eax
        jmp 0x00:real_16

        real_16:
        xor ax, ax
        mov ds, ax
        mov ss, ax

        ; Replace IDT for the interrupt
        sidt [g_storedIdt]
        lidt [g_biosIdt]

        ; Set registers
        push word [saved_EFLAGS]
        popf

        mov ax, [saved_AX]
        mov cx, [saved_CX]
        mov dx, [saved_DX]
        mov bx, [saved_BX]
        mov si, [saved_SI]
        mov di, [saved_DI]
        mov es, [saved_ES]
        mov fs, [saved_FS]
        mov gs, [saved_GS]

        ; Execute interrupt
        interrupt_opcode: db 0xCD
        interrupt_number: db 0x00

        ; Read registers
        pushf
        pop word [saved_EFLAGS]

        mov [saved_AX], ax
        mov [saved_CX], cx
        mov [saved_DX], dx
        mov [saved_BX], bx
        mov [saved_SI], si
        mov [saved_DI], di
        mov [saved_ES], es
        mov [saved_FS], fs
        mov [saved_GS], gs

        ; Restore previous IDT
        lidt [g_storedIdt]

        ; Back to protected
        mov eax, cr0
        or eax, 1
        mov cr0, eax

        push word [g_gdtInfo_selectorCode32]
        push word protected_32
        retf

        protected_32:
        [bits 32]
        mov eax, [g_gdtInfo_selectorData32]
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov esp, [stackCache]
        pop ebp
        ret