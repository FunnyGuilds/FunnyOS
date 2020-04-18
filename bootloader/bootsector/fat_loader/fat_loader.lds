OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x500;

    .text :
    {
        *(.intro)
        *(.text*)
    }

    . = ALIGN(0x200);

    .rodata :
    {
        *(.rodata*)
    }

    . = ALIGN(0x200);

    .data :
    {
        *(.data*)
    }

    . = ALIGN(0x200);

    .bss :
    {
        *(.bss*)
    }

    . = 0x500 + 16 * 0x200 - 4;

    .magic :
    {
        *(.magic)
    }

    /DISCARD/ :
    {
        *(.*)
    }
}