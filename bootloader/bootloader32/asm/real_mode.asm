[bits 32]

SECTION .real.text
    GLOBAL do_real_mode_interrupt
    do_real_mode_interrupt:
        ; Save registers, prepare new stack
        push ebp
        mov [stack_cache], esp
        lea esp, [stack16_end]

        ; Update int opcode
        mov eax, [stack_cache]
        mov eax, [eax + 4 * 2]
        mov [.interrupt_number], al

        ; Jump to 16-bit mode
        jmp 0x18:.protected_16

        [bits 16]
        .protected_16:
            ; Now to real mode
            mov eax, cr0
            and eax, ~1
            mov cr0, eax
            jmp 0x00:.real_16

        .real_16:
            xor ax, ax
            mov ds, ax
            mov ss, ax

            ; Store GDT because some BIOSes may trash it.
            sgdt [stored_gdt]

            ; Replace IDT for the interrupt
            sidt [stored_idt]
            lidt [bios_idt]

            ; Set registers
            push dword [saved_EFLAGS]
            popfd

            mov eax, [saved_EAX]
            mov ecx, [saved_ECX]
            mov edx, [saved_EDX]
            mov ebx, [saved_EBX]
            mov ebp, [saved_EBP]
            mov esi, [saved_ESI]
            mov edi, [saved_EDI]
            mov es,  [saved_ES]
            mov fs,  [saved_FS]
            mov gs,  [saved_GS]

            ; Execute interrupt
            .interrupt_opcode: db 0xCD
            .interrupt_number: db 0x00

            ; Read registers
            pushfd
            pop dword [saved_EFLAGS]

            mov [saved_EAX], eax
            mov [saved_ECX], ecx
            mov [saved_EDX], edx
            mov [saved_EBX], ebx
            mov [saved_EBP], ebp
            mov [saved_ESI], esi
            mov [saved_EDI], edi
            mov [saved_ES ], es
            mov [saved_FS ], fs
            mov [saved_GS ], gs

            ; Restore previous IDT and GDT
            lidt [stored_idt]
            lgdt [stored_gdt]

            ; Back to protected
            mov eax, cr0
            or eax, 1
            mov cr0, eax
            jmp 0x08:.protected_32

        [bits 32]
        .protected_32:
            mov ax, 0x10
            mov ds, ax
            mov ss, ax
            mov es, ax
            mov fs, ax
            mov gs, ax

            mov esp, [stack_cache]
            pop ebp
            ret
        
SECTION .real.data
    ; BIOS IDT
    bios_idt:
        dw (256 * 4) - 1
        dd 0x00000000

    ; Temporary IDT storage.
    stored_idt:
        dw 0x00
        dd 0x00

    ; Temporary GDT storage.
    stored_gdt:
        dw 0x00
        dd 0x00

    ; Cache for 32-bit stack pointer.
    stack_cache: dd 0x00

    ; Stack for 16-bit mode. Linker will put it somewhere low in memory.
    stack16:
        times (0x400 * 4) db 0x00       ; 4KB stack
    stack16_end:

    ; 16-bit mode registers status
    GLOBAL g_savedRegisters:
    g_savedRegisters:
        saved_EAX:     dd 0x00
        saved_ECX:     dd 0x00
        saved_EDX:     dd 0x00
        saved_EBX:     dd 0x00
        saved_EBP:     dd 0x00
        saved_ESI:     dd 0x00
        saved_EDI:     dd 0x00
        saved_EFLAGS:  dd 0x00
        saved_ES:      dw 0x00
        saved_FS:      dw 0x00
        saved_GS:      dw 0x00

    ; Buffer for real mode data
    GLOBAL g_realBuffer
    GLOBAL g_realBufferTop
    g_realBuffer:
        times 0x1000 db 0x00
    g_realBufferTop: