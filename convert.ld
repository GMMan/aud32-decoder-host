ENTRY(main)
SECTIONS
{
    . = 0x0001d100;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    . = 0x20000800;
    .data : { *(.data) }
    .bss : { *(.bss) }
}

aud32dec_initialise = 0x019c0;
aud32dec_decode = 0x0284e;
