[bits 16]

SECTION .text
    GLOBAL make_memory_map
    make_memory_map:
        ; Counter
        push bp
        mov bp, sp
        sub sp, 0x03

        mov [bp - 0x02], word 0     ; Entry count
        mov [bp - 0x03], byte 0     ; Has ACPI extended attributes

        mov di, 0x50
        mov es, di
        xor di, di

        mov ebx, 0x00

        make_memory_map_loop:
            ; Call the interrupt
            mov eax, 0xE820
            mov edx, 0x534D4150
            mov ecx, 24
            int 0x15
            jc make_memory_map__ret

            ; Counter increment and move memory
            inc word [bp - 0x02]
            add di, 24

            ; Check if we had ACPI extended attributes
            cmp ecx, 24
            jl make_memory_map_loop_continue

            ; We have attributes
            mov [bp - 0x03], byte 1

            make_memory_map_loop_continue:
            ; Check if we are done
            cmp ebx, 0
            jne make_memory_map_loop

        make_memory_map__ret:
            mov ebx, 0x500
            mov cx, word [bp - 0x02]
            mov al, byte [bp - 0x03]

            xor di, di
            mov es, di
            clc
            leave
            ret
