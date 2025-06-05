#include <stdint.h>

#define SECRET_ADDR     (0x80400000)

/* Function declared in assembly */
void uart_putchar(char c);

/* Function to print a string using UART */
void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putchar(*s++);
    }
}

/* Helper to print in hex */
void print_hex(unsigned long value)
{
    char hex_chars[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 15; i >= 0; i--)
    {
        int nibble = (value >> (i * 4)) & 0xF;
        uart_putchar(hex_chars[nibble]);
    }
}

/* Global variable we should be able to access */
volatile unsigned long test_var = 0xDEADBEEF;

void uart_puthex64(uint64_t val)
{
    const char hex[] = "0123456789ABCDEF";
    for (int i = 15; i >= 0; i--)
    {
        uart_putchar(hex[(val >> (i * 4)) & 0xF]);
    }
}
void m_mode_trap_handler(void);
/* Helper to print PMPADDR registers */
void read_pmpaddr(int index)
{
    if (index < 0 || index > 15)
        return;

    uint64_t value;
    uart_puts("pmpaddr");
    uart_putchar('0' + index / 10);
    uart_putchar('0' + index % 10);
    uart_puts(": 0x");

    switch (index)
    {
    case 0:
        asm volatile("csrr %0, 0x3B0" : "=r"(value));
        break;
    case 1:
        asm volatile("csrr %0, 0x3B1" : "=r"(value));
        break;
    case 2:
        asm volatile("csrr %0, 0x3B2" : "=r"(value));
        break;
    case 3:
        asm volatile("csrr %0, 0x3B3" : "=r"(value));
        break;
    case 4:
        asm volatile("csrr %0, 0x3B4" : "=r"(value));
        break;
    case 5:
        asm volatile("csrr %0, 0x3B5" : "=r"(value));
        break;
    case 6:
        asm volatile("csrr %0, 0x3B6" : "=r"(value));
        break;
    case 7:
        asm volatile("csrr %0, 0x3B7" : "=r"(value));
        break;
    case 8:
        asm volatile("csrr %0, 0x3B8" : "=r"(value));
        break;
    case 9:
        asm volatile("csrr %0, 0x3B9" : "=r"(value));
        break;
    case 10:
        asm volatile("csrr %0, 0x3BA" : "=r"(value));
        break;
    case 11:
        asm volatile("csrr %0, 0x3BB" : "=r"(value));
        break;
    case 12:
        asm volatile("csrr %0, 0x3BC" : "=r"(value));
        break;
    case 13:
        asm volatile("csrr %0, 0x3BD" : "=r"(value));
        break;
    case 14:
        asm volatile("csrr %0, 0x3BE" : "=r"(value));
        break;
    case 15:
        asm volatile("csrr %0, 0x3BF" : "=r"(value));
        break;
    }

    uart_puthex64(value);
    uart_puts("\r\n");
}

static inline void read_csr(uint32_t csr_id)
{
    uart_puts("pmpcfg");
    uint64_t value;
    switch (csr_id)
    {
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

void test_pmpcfg2_writes()
{
    uart_puts("\r\n--- PMP TEST: WRITING TO pmpcfg2 ---\r\n");

    // Use 8 distinct byte values so we can clearly see what sticks
    // Byte 0 = 0xA1, Byte 1 = 0xB2, ..., Byte 7 = 0xF8
    uint64_t test_val = 0xF8E7D6C5B4A39281UL;

    asm volatile("csrw 0x3A2, %0" ::"r"(test_val));

    uart_puts("Wrote pmpcfg2 = 0x");
    uart_puthex64(test_val);
    uart_puts("\r\n");

    // Now read it back
    uint64_t readback;
    asm volatile("csrr %0, 0x3A2" : "=r"(readback));

    uart_puts("Read  pmpcfg2 = 0x");
    uart_puthex64(readback);
    uart_puts("\r\n");

    // Dump each byte
    for (int i = 0; i < 8; i++)
    {
        uint8_t b = (readback >> (i * 8)) & 0xFF;
        uart_puts("Byte ");
        uart_putchar('0' + i);
        uart_puts(": 0x");
        uart_putchar("0123456789ABCDEF"[b >> 4]);
        uart_putchar("0123456789ABCDEF"[b & 0xF]);
        uart_puts("\r\n");
    }

    uart_puts("--- END TEST ---\r\n");
}

/* Main entry point */
void c_entry(void)
{
    uart_puts("Hello C-code!\r\n");

    /* First read from our own data variable (should succeed) */
    uart_puts("Reading from our own data variable: ");
    print_hex(test_var);
    uart_puts("\r\n");

    /* Try reading from the code section with a byte-by-byte approach */
    uart_puts("\r\nReading first 16 bytes from 0x80200000 one byte at a time:\r\n");

    volatile unsigned char *code_ptr = (volatile unsigned char *)0x80200000;
    for (int i = 0; i < 16; i++)
    {
        uart_puts("Byte ");
        uart_putchar('0' + i / 10);
        uart_putchar('0' + i % 10);
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
    for (volatile int i = 0; i < 1000000; i++)
    {
    }

    /* This access should trigger a trap due to PMP protection */
    volatile unsigned long *opensbi_ptr = (volatile unsigned long *)0x80000000;
    unsigned long value = *opensbi_ptr; /* This should reboot the system */

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

    uart_puts("\r\nDumping all PMPADDR registers:\r\n");

    for (int i = 0; i < 16; i++)
    {
        read_pmpaddr(i);
    }

    uart_puts("\r\nSetting new PMP entries...\r\n");

    // Set pmpaddr4 = 0xF0000000 ~ 0xF7FFFFFF (128MB NAPOT)
    // NAPOT encoded as: base_addr | ((size-1) >> 3)
    uint64_t addr4 = (0xF0000000 >> 2) | ((0x8000000 - 1) >> 3);
    asm volatile("csrw 0x3B4, %0" ::"r"(addr4)); // pmpaddr4

    // Set pmpaddr5 = 0xFFF73000 ~ 0xFFF74000 (4KB NAPOT)
    uint64_t addr5 = (0xFFF73000 >> 2) | ((0x1000 - 1) >> 3);
    asm volatile("csrw 0x3B5, %0" ::"r"(addr5)); // pmpaddr5

    // Configure PMP4 in pmpcfg0: index 4 → byte offset = 4
    // R/W, A=NAPOT => 0b011 = 0x18 (RW) + 0x18 << 32
    uint64_t new_pmpcfg0 = 0x0000001800000000UL | 0x00000000001F1818UL;
    asm volatile("csrw pmpcfg0, %0" ::"r"(new_pmpcfg0));

    // Configure PMP5 in pmpcfg2: index 5 → byte offset = 1
    uint64_t new_pmpcfg2 = 0x0000000000000018UL;
    asm volatile("csrw 0x3A2, %0" ::"r"(new_pmpcfg2)); // pmpcfg2

    uart_puts("Re-reading PMP configuration:\r\n");
    read_csr(0);
    read_csr(2);

    uart_puts("Re-dumping PMPADDR registers:\r\n");
    for (int i = 0; i < 16; i++)
    {
        read_pmpaddr(i);
    }

    test_pmpcfg2_writes();

    uart_puts("Locking secret_data region with PMP...\r\n");

    volatile uint8_t secret_local_data[8];
    for (int i = 0; i < 8; i++)
    {
        secret_local_data[i] = i & 0xFF;
    }

    uint64_t addr = (uintptr_t)SECRET_ADDR;
    uint64_t addr2 = (uintptr_t)&secret_local_data;

    addr2 -= 0x1000;

    // Calculate NAPOT encodings
    uint64_t secret_napot = (addr >> 2) | ((0x1000UL - 1) >> 3);         // 4KB NAPOT for secret_data
    uint64_t local_napot = (addr2 >> 2) | ((0x1000UL - 1) >> 3);         // 4KB NAPOT for local data
    uint64_t text_napot = (0x80200000 >> 2) | ((0x1000UL - 1) >> 3);     // 4KB NAPOT for the code
    uint64_t code_napot = (0x80700000UL >> 2) | ((0x100000UL - 1) >> 3); // 1MB M-mode stack region
    uint64_t uart_napot = (0x04140000UL >> 2) | ((0x1000UL - 1) >> 3);   // 4KB UART

    // CRITICAL: Put secret_data protection in PMP0 (highest priority)
    asm volatile("csrw pmpaddr0, %0" ::"r"(secret_napot));      // PMP0: secret_data (DENY)
    // asm volatile("csrw pmpaddr1, %0" ::"r"(local_napot));       // PMP1: local data (ALLOW)
    asm volatile("csrw pmpaddr1, %0" ::"r"(text_napot));       // PMP1: code region (DENY)
    asm volatile("csrw pmpaddr2, %0" ::"r"(code_napot));        // PMP2: M-mode stack region (DENY)
    asm volatile("csrw pmpaddr3, %0" ::"r"(uart_napot));        // PMP3: UART (ALLOW)
    asm volatile("csrw pmpaddr4, %0" ::"r"(0x3FFFFFFFFFUL));    // PMP4: all regions (ALLOW)

    // Configure pmpcfg0: PMP0=deny, PMP1/2/3=allow
    uint64_t cfg = (0x18UL <<  0) |     // PMP0: NAPOT, no RWX (DENY secret_data)
                   (0x1CUL <<  8) |     // PMP1: NAPOT, X (DENY R/W in code region)
                   (0x18UL << 16) |     // PMP2: NAPOT, RWX (DENY M-mode stack)
                   (0x1FUL << 24) |     // PMP3: NAPOT, RWX (ALLOW UART)
                   (0x1FUL << 32) ;     // PMP4: NAPOT, RWX (ALLOW all memory)

    asm volatile("csrw pmpcfg0, %0" ::"r"(cfg));

    uart_puts("\r\n secret addr: ");
    print_hex(addr);
    uart_puts("\r\n secret_napot: ");
    print_hex(secret_napot);
    uart_puts("\r\n local_secret addr: ");
    print_hex(addr2);
    uart_puts("\r\n local_napot: ");
    print_hex(local_napot);
    print_hex(local_napot);
    uart_puts("\r\n code_napot: ");
    print_hex(code_napot);
    uart_puts("\r\n uart_napot: ");
    print_hex(uart_napot);
    uart_puts("\r\n cfg: ");
    print_hex(cfg);
    uart_puts("\r\n");

    read_csr(0);
    uart_puts("\r\nDumping all PMPADDR registers:\r\n");
    for (int i = 0; i < 16; i++)
    {
        read_pmpaddr(i);
    }
    uart_puts("\r\n");

    uart_puts("Reading secret_data from M-mode.\r\n Value: ");
    print_hex(*(uint64_t *)SECRET_ADDR);
    uart_puts("\r\n Address: ");
    print_hex((uint64_t *)SECRET_ADDR);
    uart_puts("\r\n");

    uart_puts("Reading secret_local_data from M-mode.\r\n Value: ");
    print_hex(*(uint64_t *)secret_local_data);
    uart_puts("\r\n Address: ");
    print_hex((uint64_t *)secret_local_data);
    uart_puts("\r\n");


    uint64_t medeleg;
    asm volatile("csrr %0, medeleg" : "=r"(medeleg));
    medeleg &= ~(1UL << 9);
    asm volatile("csrw medeleg, %0" ::"r"(medeleg));

    // Set mtvec to trap handler for M-mode
    asm volatile("csrw mtvec, %0" ::"r"(m_mode_trap_handler));

    uart_puts("Now dropping to S-mode...\r\n");

    asm volatile("la sp, s_mode_stack_top");

    extern void s_mode_entry(void);
    uint64_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));
    mstatus = (mstatus & ~(3UL << 11)) | (1UL << 11); // MPP = S-mode
    asm volatile("csrw mstatus, %0" ::"r"(mstatus));
    asm volatile("csrw mepc, %0" ::"r"(s_mode_entry));
    asm volatile("mret");

    while (1)
    {
    }
}

void s_mode_main(void)
{
    uart_puts("\r\nHello from S-mode!\r\n");
    // uart_puts("Attempting to read secret_data from (should not work) S-mode!\r\n");
    // volatile uint64_t addr = SECRET_ADDR; // this should trap
    // uart_puts("Read secret_data succeeded?\r\n Value: ");
    // print_hex(*(uint64_t *)addr);
    // uart_puts("\r\n Address: ");
    // print_hex(addr);
    // uart_puts("\r\n");

    // uart_puts("Attempting to read M-mode stack from (should not work) S-mode!\r\n");
    // volatile uint64_t addr = 0x00000000807FFF48;
    // uart_puts("Read local_secret_data succeeded?\r\n Value: ");
    // print_hex(*(uint64_t *)addr);
    // uart_puts("\r\n Address: ");
    // print_hex(addr);
    // uart_puts("\r\n");

    uart_puts("Calling back to M-mode with ecall...\r\n");
    asm volatile("ecall");

    // this should never be reached if trap handled correctly
    uart_puts("Returned from ecall?!\r\n");

    while (1)
        ;
}

__attribute__((aligned(4))) void m_mode_trap_handler(void)
{
    uart_puts("Back in M-mode via trap or ecall!\r\n");

    uint64_t mcause, mepc, mtval;

    // Read M-mode trap CSRs
    asm volatile("csrr %0, 0x342" : "=r"(mcause)); // mcause
    asm volatile("csrr %0, 0x341" : "=r"(mepc));   // mepc
    asm volatile("csrr %0, 0x343" : "=r"(mtval));  // mtval

    uart_puts("M-mode trap reason:\r\n");
    uart_puts("mcause: ");
    print_hex(mcause);
    uart_puts("\r\n");
    uart_puts("mepc:   ");
    print_hex(mepc);
    uart_puts("\r\n");
    uart_puts("mtval:  ");
    print_hex(mtval);
    uart_puts("\r\n");

    uart_puts("Hello again from M-mode!\r\n");

    // Print secret_data (global array)
    uart_puts("M-mode reading secret_data: ");
    print_hex(*(uint64_t *)SECRET_ADDR);
    uart_puts(" at address: ");
    print_hex((uint64_t)SECRET_ADDR);
    uart_puts("\r\n");

    // Print secret_local_data (from M-mode stack - hardcoded address)
    uart_puts("M-mode reading secret_local_data: ");
    volatile uint64_t *local_ptr = (volatile uint64_t *)0x00000000807FFF40;
    print_hex(*local_ptr);
    uart_puts(" at address: ");
    print_hex((uint64_t)local_ptr);
    uart_puts("\r\n");

    // c_entry(); infinite
    while (1)
    {
        asm volatile("wfi");
    }
}
