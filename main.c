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

    /* First read from our own data (this should succeed) */
    uart_puts("Reading from our own data variable: ");
    print_hex(test_var);
    uart_puts("\n");

    /* Read from our stack (this should also succeed) */
    unsigned long stack_var = 0xCAFEBABE;
    uart_puts("Reading from our stack variable: ");
    print_hex(stack_var);
    uart_puts("\n\n");

    /* Small delay to ensure the above gets printed */
    for (volatile int i = 0; i < 1000000; i++) {}

    uart_puts("PMP Demo: Attempting to read from OpenSBI memory...\n");

    /* This access should trigger a trap due to PMP protection */
    volatile unsigned long *opensbi_ptr = (volatile unsigned long *)0x80000000;
    unsigned long value = *opensbi_ptr;  /* This should reboot the system */

    /* If we get here, protection failed */
    uart_puts("Protection failed - system did not trap!\n");
    uart_puts("Value read: ");
    print_hex(value);
    uart_puts("\n");

    while(1) {}
}
