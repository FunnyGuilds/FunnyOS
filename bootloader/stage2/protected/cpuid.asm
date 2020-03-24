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

p_check_long_mode_supported:
    pushad

    mov esi, p_message_lm_test
    call p_print

    ; Check for extended processor info and feature bits
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jl p_check_long_mode_supported__fail

    ; Check for LM flag
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz p_check_long_mode_supported__fail

    ; Success
    clc

    p_check_long_mode_supported__ret:
        popad
        ret

    p_check_long_mode_supported__fail:
       stc
       jmp p_check_long_mode_supported__ret


p_message_cpuid_test:           db "Testing for CPUID", 0
p_message_lm_test:              db "Testing for Long mode support", 0

