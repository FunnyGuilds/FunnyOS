OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x4000;

    .intro :
    {
        KEEP(*(.intro))
    }

    .real.text :
    {
        *(.real.text*)
    }

    .real.data :
    {
        *(.real.data*)
    }

    .text :
    {
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

    . = 0x4000 + 128 * 0x200 - 4;

    .magic :
    {
        KEEP(*(.magic))
    }

    . = ADDR(.bss) + SIZEOF(.bss);
    . = ALIGN(0x400);
}