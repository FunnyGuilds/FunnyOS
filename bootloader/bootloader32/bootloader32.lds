OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x500;

    .text :
    {
        *(.intro)
        *(.text*)
    }

    . = ALIGN(0x400);

    .rodata :
    {
        *(.rodata*)
    }

    . = ALIGN(0x400);

    .data :
    {
        *(.data)
    }

    . = ALIGN(0x400);

    .bss :
    {
        *(.bss)
    }

    . = 0x500 + 30 * 0x200 - 4;

    .magic :
    {
        *(.magic)
    }

    /DISCARD/ :
    {
        *(.*)
    }
}