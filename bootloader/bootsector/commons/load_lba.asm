[bits 16]

SECTION .text
    ;
    ; Utility function for loading sectors from LBA addresses in real mode.
    ; Parameters:
    ;   EAX - the LBA address
    ;   DL - drive number
    ;   CL - number of sectors to load
    ;   ES:BX - memory location
    ;
    ; Output:
    ;   On success: CF clear
    ;   On fail: CF set, error code in AH
    ;
    GLOBAL load_lba
    load_lba:
        mov     [edd_packet.number_of_blocks], cl
        mov     [edd_packet.destination_offset], bx
        mov     [edd_packet.destination_segment], es
        mov     [edd_packet.startlba_low], eax

        push    ds
        push    si

        mov     ah, 0x42
        xor     si, si
        mov     ds, si
        mov     si, edd_packet
        int     0x13

        pop     si
        pop     ds
        ret

SECTION .data
    edd_packet:
        .packet_size:          db 0x10
        .reserved:             db 0
        .number_of_blocks:     db 0
        .reserved2:            db 0
        .destination_offset:   dw 0
        .destination_segment:  dw 0
        .startlba_low:         dd 0
        .startlba_high:        dd 0
