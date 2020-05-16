OUTPUT_FORMAT(binary)

SECTIONS
{
    .text :
    {
        *(.text*)
    }

    .data :
    {
        *(.data*)
    }

    /DISCARD/ :
    {
        *(.*)
    }
}