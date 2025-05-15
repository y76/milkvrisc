/* Function declared in assembly */
void uart_putchar(char c);

/* Function to print a string using UART */
void uart_puts(const char *s) {
    while (*s) {
        uart_putchar(*s++);
    }
}

/* Helper to print in hex */
void print_hex(unsigned long value) {
    char hex_chars[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 15; i >= 0; i--) {
        int nibble = (value >> (i * 4)) & 0xF;
        uart_putchar(hex_chars[nibble]);
    }
}

/* Global variable we should be able to access */
volatile unsigned long test_var = 0xDEADBEEF;

/* Main entry point */
void c_entry(void) {
    uart_puts("Hello C-code!\n\n");

    /* First read from our own data variable (should succeed) */
    uart_puts("Reading from our own data variable: ");
    print_hex(test_var);
    uart_puts("\n");

    /* Try reading from the code section with a byte-by-byte approach */
    uart_puts("\nReading first 16 bytes from 0x80200000 one byte at a time:\n");

    volatile unsigned char *code_ptr = (volatile unsigned char *)0x80200000;
    for (int i = 0; i < 16; i++) {
        uart_puts("Byte ");
        uart_putchar('0' + i/10);
        uart_putchar('0' + i%10);
        uart_puts(": ");

        /* Read one byte at a time */
        unsigned char byte = code_ptr[i];

        uart_puts("0x");
        unsigned char nibble = (byte >> 4) & 0xF;
        uart_putchar(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
        nibble = byte & 0xF;
        uart_putchar(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
        uart_puts("\n");
    }

    uart_puts("\nIf you see this message, reading from 0x80200000 succeeded!\n");
    uart_puts("Now attempting to read from protected OpenSBI memory...\n");

    /* Small delay to ensure the above gets printed */
    for (volatile int i = 0; i < 1000000; i++) {}

    /* This access should trigger a trap due to PMP protection */
    volatile unsigned long *opensbi_ptr = (volatile unsigned long *)0x80000000;
    unsigned long value = *opensbi_ptr;  /* This should reboot the system */

    /* If we get here, protection failed */
    uart_puts("Protection failed - system did not trap!\n");

    while(1) {}
}
