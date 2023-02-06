/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

/* Reference: https://wiki.osdev.org/PIC */
 
#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
	unsigned long flags;
	cli_and_save(flags);

	/*Mask the data*/
	outb(master_mask, MASTER_8259_DATA);
	outb(slave_mask, SLAVE_8259_DATA);

	/*starts the initialization sequence in cascade mode*/
	outb(ICW1, MASTER_8259_COMMAND);
	outb(ICW1, SLAVE_8259_COMMAND);

	/*Set the offset of PIC vectors, which is ICW2*/
	outb(ICW2_MASTER, MASTER_8259_DATA);
	outb(ICW2_SLAVE, SLAVE_8259_DATA);

	/*ICW3_MASTER = 0x04 indicates a slave PIC located at irq2*/
	outb(ICW3_MASTER, MASTER_8259_DATA);
	/*ICW3_SLAVE = 0x02 indicates the slave ID*/
	outb(ICW3_SLAVE, SLAVE_8259_DATA);

	/*Set 8295A PIC for 8086 system operation*/
	outb(ICW4, MASTER_8259_DATA);
	outb(ICW4, SLAVE_8259_DATA);

	/*Unmask the IRQ for the slave PIC*/
	enable_irq(IRQ_SLAVE_8259);
	
	/*Enable the interrupts*/
	sti();
	restore_flags(flags);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	/*Set ~1 to mask the certain IRQ*/
	uint8_t bitmask = 0x01;

	/* If the IRQ is on the master PIC */
	if (irq_num < 8) {
		/*Set 0 to the position of the IRQ in PIC*/
		bitmask <<= irq_num;
		bitmask = ~bitmask;
		master_mask &= bitmask;
		outb(master_mask, MASTER_8259_DATA);
	}
	/* If the IRQ is on the slave PIC */
	else {
		/*Substract the offset of eight IRQs in Master PIC*/
		irq_num -= 8;

		/*Set 0 to the position of the IRQ in PIC*/
		bitmask <<= irq_num;
		bitmask = ~bitmask;
		slave_mask &= bitmask;
		outb(slave_mask, SLAVE_8259_DATA);
	}
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	/*Set 1 to mask the certain IRQ*/
	uint8_t bitmask = 0x01;

	/* If the IRQ is on the master PIC */
	if (irq_num < 8) {
		/*Set 1 to the position of the IRQ in master PIC*/
		bitmask <<= irq_num;
		master_mask &= bitmask;
		outb(master_mask, MASTER_8259_DATA);
	}
	/* If the IRQ is on the slave PIC */
	else {
		/*Substract the offset of eight IRQs in Master PIC*/
		irq_num -= 8;

		/*Set 1 to the position of the IRQ in slave PIC*/
		bitmask <<= irq_num;
		slave_mask &= bitmask;
		outb(slave_mask, SLAVE_8259_DATA);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	
	/* If the IRQ is on the master PIC */
	if (irq_num < 8) {
		/* Send EOI to the corresponding IRQ on the master PIC */
		outb(irq_num | EOI, MASTER_8259_COMMAND);
	}
	/* If the IRQ is on the slave PIC */
	else {
		/* Send EOI to the corresponding IRQ on the slave PIC */
		outb((irq_num-8) | EOI, SLAVE_8259_COMMAND);
		/* Send EOI to the IRQ for the slave PIC */
		outb(IRQ_SLAVE_8259 | EOI, MASTER_8259_COMMAND);
	}
}
