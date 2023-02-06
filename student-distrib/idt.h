#ifndef _IDT_H
#define _IDT_H

#include "lib.h"
#include "i8259.h"

#define   IDT_SYSCALL   0x80 // Use 0x80 for system calls
/* Initializes IDT */
extern void idt_init();
/* Add device to irq */
void add_irq(int irq_num, void (*handler));
#endif
