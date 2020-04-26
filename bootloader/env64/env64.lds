OUTPUT_FORMAT(binary)

SECTIONS
{
    .entry :
    {
        *(.entry)
    }

    . = ALIGN(0x1000);

    .text :
    {
        *(.text*)
    }

    TEXT_OFFSET = ADDR(.text);
    SIZE = .;

    /DISCARD/ :
    {
        *(.*)
    }
}