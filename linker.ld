ENTRY(_start)

SECTIONS
{
    . = 0x80200000;

    .text : {
        *(.text.startup)
        *(.text)
        *(.text.*)
    }

    . = 0x80201000;

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

    /* Stack and end-of-image markers */
    . = ALIGN(8);
    _end = .;

    . = 0x81000000;
    . = ALIGN(16);
    s_mode_stack_top = . + 0x1000; /* 4KB S-mode stack */
}
