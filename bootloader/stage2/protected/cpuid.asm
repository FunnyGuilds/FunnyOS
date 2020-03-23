p_test_for_cpuid:
    pushad
    pushfd

    mov esi, p_message_cpuid_test
    call p_print

    pushfd
    mov edx, dword [esp]         ; save for comparison
    xor dword [esp], 0x00200000  ; Flip ID bit

    popfd
    pushfd
    pop eax

    popfd

    cmp eax, edx
    jne p_test_for_cpuid__supported

    ; not supported
    stc
    jmp p_test_for_cpuid__ret

    p_test_for_cpuid__supported:
    clc

    p_test_for_cpuid__ret:
    popad
    ret

p_print_cpuid_info:
    pushad

    mov esi, p_print_cpuid_info_msg_header
    call p_print

    mov eax, 0x00
    cpuid

    mov edi, p_print_cpuid_info_msg_manufacturer
    add edi, 17
    mov [edi + 0x00], ebx
    mov [edi + 0x04], edx
    mov [edi + 0x08], ecx

    mov esi, p_print_cpuid_info_msg_manufacturer
    call p_print

    popad
    ret

    p_print_cpuid_info_msg_header:              db "CPUID information: ", 0
    p_print_cpuid_info_msg_manufacturer:        db " - Manufacturer: XXXXYYYYZZZZ", 0

p_message_cpuid_test:           db "Testing for CPUID", 0
