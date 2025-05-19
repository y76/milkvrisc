#include <stdint.h>

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

void uart_puthex64(uint64_t val) {
    const char hex[] = "0123456789ABCDEF";
    for (int i = 15; i >= 0; i--) {
        uart_putchar(hex[(val >> (i * 4)) & 0xF]);
    }
}

static inline void read_csr(uint32_t csr_id) {
    uart_puts("pmpcfg");
    uint64_t value;
    switch (csr_id) {
        case 0:
            asm volatile("csrr %0, pmpcfg0" : "=r"(value));
            uart_putchar('0');
            break;
        case 1:
            asm volatile("csrr %0, pmpcfg1" : "=r"(value));
            uart_putchar('1');
            break;
        case 2:
            asm volatile("csrr %0, pmpcfg2" : "=r"(value));
            uart_putchar('2');
            break;
        case 3:
            asm volatile("csrr %0, pmpcfg3" : "=r"(value));
            uart_putchar('3');
            break;
        case 4:
            // asm volatile("csrr %0, pmpcfg4" : "=r"(value));
            asm volatile("csrr %0, 0x3A4" : "=r"(value));
            uart_putchar('4');
            break;
        case 5:
            // asm volatile("csrr %0, pmpcfg5" : "=r"(value));
            asm volatile("csrr %0, 0x3A5" : "=r"(value));
            uart_putchar('5');
            break;
        case 6:
            // asm volatile("csrr %0, pmpcfg6" : "=r"(value));
            asm volatile("csrr %0, 0x3A6" : "=r"(value));
            uart_putchar('6');
            break;
        case 7:
            // asm volatile("csrr %0, pmpcfg7" : "=r"(value));
            asm volatile("csrr %0, 0x3A7" : "=r"(value));
            uart_putchar('7');
            break;
        // Add more if needed
    }
    uart_puts(": 0x");
    uart_puthex64(value);
    uart_puts("\r\n");
}

/* Main entry point */
void c_entry(void) {
    uart_puts("Hello C-code!\r\n");

    /* First read from our own data variable (should succeed) */
    uart_puts("Reading from our own data variable: ");
    print_hex(test_var);
    uart_puts("\r\n");

    /* Try reading from the code section with a byte-by-byte approach */
    uart_puts("\r\nReading first 16 bytes from 0x80200000 one byte at a time:\r\n");

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
        uart_puts("\r\n");
    }

    uart_puts("\r\nIf you see this message, reading from 0x80200000 succeeded!\r\n");
    uart_puts("Now attempting to read from protected OpenSBI memory...\r\n");

    /* Small delay to ensure the above gets printed */
    for (volatile int i = 0; i < 1000000; i++) {}

    /* This access should trigger a trap due to PMP protection */
    volatile unsigned long *opensbi_ptr = (volatile unsigned long *)0x80000000;
    unsigned long value = *opensbi_ptr;  /* This should reboot the system */

    /* If we get here, protection failed */

    uart_puts("Protection failed - system did not trap!\r\n");
    uart_puts("Protection didn't actually fail - we are in M-Mode!\r\n");

    read_csr(0);
    // read_csr(1);
    read_csr(2);
    // read_csr(3);
    // read_csr(4);
    // read_csr(5);
    // read_csr(6);
    // read_csr(7);

    while(1) {}
}
