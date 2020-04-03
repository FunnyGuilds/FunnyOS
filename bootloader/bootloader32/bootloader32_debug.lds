OUTPUT_FORMAT(elf32-i386)

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
}