OUTPUT_FORMAT(binary)

SECTIONS
{
    .bios_parameter_block(0x7C00) :
    {
        *(.bios_parameter_block)
    }

    .bootsector(0x7C00 + 0x05A) :
    {
        *(.intro)
        *(.text)
        *(.rodata)
        *(.data)
        *(.bss)
    }

    .boot_signature(0x7C00 + 0x200 - 2) :
    {
        *(.boot_signature)
    }

    /DISCARD/ :
    {
        *(.*)
    }
}