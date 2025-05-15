#define UART0_THR 0x04140000
#define UART0_LSR 0x04140014

	.section .text
	.global _start
_start:
	/* BL33 information */
	j real_start
	.balign 4
	.word 0x33334c42  /* b'BL33' */
	.word 0xdeadbeea  /* CKSUM */
	.word 0xdeadbeeb  /* SIZE */
	.quad 0x80200000  /* RUNADDR */
	.word 0xdeadbeec
	.balign 4
	j real_start
	.balign 4
	/* Information end */

real_start:
	/* Set up stack pointer */
	li sp, 0x80400000  /* Set stack pointer to some address */

	la s0, str
1:
	lbu a0, (s0)
	beqz a0, call_c_code
	jal ra, uart_send
	addi s0, s0, 1
	j 1b

call_c_code:
	/* Call C function */
	jal ra, c_entry

exit:
	j exit

/* Function to be called from C code */
	.global uart_putchar
uart_putchar:
	/* Save return address */
	addi sp, sp, -16
	sd ra, 8(sp)

	/* Call uart_send with the character in a0 */
	jal ra, uart_send

	/* Restore return address */
	ld ra, 8(sp)
	addi sp, sp, 16
	ret

uart_send:
	/* Wait for tx idle */
	li t0, UART0_LSR
	lw t1, (t0)
	andi t1, t1, 0x20
	beqz t1, uart_send
	/* Send a char */
	li t0, UART0_THR
	sw a0, (t0)
	ret

.section .rodata
str:
	.asciz "Hello Milkv-duo!\n"
