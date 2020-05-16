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
        mov [edd_packet__numberofblocks], cl
        mov [edd_packet__destinationoffset], bx
        mov [edd_packet__destinationsegment], es
        mov [edd_packet__startlba_low], eax
        push ds
        push si
        mov ah, 0x42
        xor si, si
        mov ds, si
        mov si, edd_packet
        int 0x13
        pop si
        pop ds
        ret

SECTION .data
    edd_packet:
        edd_packet__packetsize:         db 0x10
        edd_packet__reserved:           db 0
        edd_packet__numberofblocks:     db 0
        edd_packet__reserved2:          db 0
        edd_packet__destinationoffset:  dw 0
        edd_packet__destinationsegment: dw 0
        edd_packet__startlba_low:       dd 0
        edd_packet__startlba_high:      dd 0