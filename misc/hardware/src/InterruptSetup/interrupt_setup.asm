%ifidn __OUTPUT_FORMAT__, elf
    [bits 32]
%elifidn __OUTPUT_FORMAT__, elf64
    [bits 64]
%else
    %fatal "Unknown output format"
%endif

DEFAULT rel

SECTION .text
    ; Define an ISR
    ; Parameter %1 - interrupt number
    ; Parameter %2 - 1 if has error code, 0 if not
    %macro ISR_HANDLER 2
        isr_routine%1:
            %if %2 == 0
                push 0
            %else
                times 2 nop
            %endif
            push %1
            jmp isr_routine
    %endmacro

    ; Standardized interrupts
    ISR_HANDLER 0 ,   0
    ISR_HANDLER 1 ,   0
    ISR_HANDLER 2 ,   0
    ISR_HANDLER 3 ,   0
    ISR_HANDLER 4 ,   0
    ISR_HANDLER 5 ,   0
    ISR_HANDLER 6 ,   0
    ISR_HANDLER 7 ,   0
    ISR_HANDLER 8 ,   1
    ISR_HANDLER 9 ,   0
    ISR_HANDLER 10,   1
    ISR_HANDLER 11,   1
    ISR_HANDLER 12,   1
    ISR_HANDLER 13,   1
    ISR_HANDLER 14,   1
    ISR_HANDLER 15,   0
    ISR_HANDLER 16,   0
    ISR_HANDLER 17,   1
    ISR_HANDLER 18,   0
    ISR_HANDLER 19,   0
    ISR_HANDLER 20,   0

    ; Reserved and user-defined
    %assign i 21
    %rep    (256 - 20)
        ISR_HANDLER i,   0
    %assign i i+1
    %endrep

    ; Make sure error, and not-error interrupts have the same size
    %if (isr_routine1 - isr_routine0) != (isr_routine11 - isr_routine10)
        %fatal "Invalid interrupt routines size"
    %endif

%if __BITS__ == 32
    %macro PUSH_REGISTERS 0
        push edi
        push esi
        push ebp
        push esp
        push ebx
        push edx
        push ecx
        push eax
    %endmacro

    %macro POP_REGISTERS 0
        pop eax
        pop ecx
        pop edx
        pop ebx
        pop esp
        pop ebp
        pop esi
        pop edi
    %endmacro

%elif __BITS__ == 64
    %macro PUSH_REGISTERS 0
        ; TODO
    %endmacro

    %macro POP_REGISTERS 0
        ; TODO
    %endmacro
%endif

    GLOBAL setup_routines:function
    setup_routines:
        %if __BITS__ == 32
            push eax
            push ebx
            mov ebx, [esp + 4 * 3]
            mov eax, [ebx]
            mov [interrupt_routine], eax
            lea eax, [REL isr_routine0]
            mov [ebx + 4], eax
            mov [ebx + 8], byte (isr_routine1 - isr_routine0)
            pop ebx
            pop eax
            ret
        %elif __BITS__ == 64
        %endif

    interrupt_routine:
        %if __BITS__ == 32
            dd 1
        %elif __BITS__ == 64
            dq 1
        %endif

    isr_routine:
        PUSH_REGISTERS
        call [interrupt_routine]
        POP_REGISTERS

        ; Cleanup the values pushed in ISR_HANDLER

        %if __BITS__ == 32
            add esp, 4 * 2
            iretd
        %elif __BITS__ == 64
            add rsp, 8 * 2
            iretq
        %endif