[bits 32]

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

    EXTERN interrupt_routine
    isr_routine:
        push edi
        push esi
        push ebp
        push esp
        push ebx
        push edx
        push ecx
        push eax
        call interrupt_routine
        pop eax
        pop ecx
        pop edx
        pop ebx
        pop esp
        pop ebp
        pop esi
        pop edi

        ; Cleanup the values pushed in ISR_HANDLER
        add esp, 4 * 2
        iretd

SECTION .rodata
    GLOBAL g_firstRoutineAddress
    g_firstRoutineAddress: dd isr_routine0

    GLOBAL g_interruptRoutineSize
    g_interruptRoutineSize: dd isr_routine1 - isr_routine0