OUTPUT_FORMAT(binary)

SECTIONS
{
    .bootsector(0x7A00) :
    {
        *(.intro)
        *(.text)
        *(.rodata)
        *(.data)
        *(.bss)
    }

    .partition_table(0x7A00 + 440) :
    {
        *(.partition_table)
    }

    /DISCARD/ :
    {
        *(.*)
    }
}