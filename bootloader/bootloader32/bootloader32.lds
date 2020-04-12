OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x500;

    .text :
    {
        KEEP(*(.intro))
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
        *(.data*)
    }

    . = ALIGN(0x400);

    .bss :
    {
        *(.bss*)
    }

    . = 0x500 + 59 * 0x200 - 4;

    .magic :
    {
        KEEP(*(.magic))
    }

    /DISCARD/ :
    {
        *(.*)
    }
}