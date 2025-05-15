ENTRY(_start)

SECTIONS
{
    . = 0x80200000;

    .text : {
        *(.text.startup)
        *(.text)
        *(.text.*)
    }

    .rodata : {
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
    }

    .data : {
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
    }

    .bss : {
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
    }

    /* Make sure everything is in range for RISC-V addressing modes */
    . = ALIGN(8);
    _end = .;
}
