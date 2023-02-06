#ifndef  _HANDLER_IN_ASBLY
#define  _HANDLER_IN_ASBLY

/* halt when exception occurs */
extern void EXC_FINISHED();

/* irq handlers */
extern void ITR_IRQ0();
extern void ITR_IRQ1();
extern void ITR_IRQ2();
extern void ITR_IRQ3();
extern void ITR_IRQ4();
extern void ITR_IRQ5();
extern void ITR_IRQ6();
extern void ITR_IRQ7();
extern void ITR_IRQ8();
extern void ITR_IRQ9();
extern void ITR_IRQA();
extern void ITR_IRQB();
extern void ITR_IRQC();
extern void ITR_IRQD();
extern void ITR_IRQE();
extern void ITR_IRQF();

/* syscall */
extern void ITR_SYS();

extern void* irq_handler_jumptable[16];
extern void* syscall_jumptable[10];
/* interrupt handlers */
extern void ITR_DE();
extern void ITR_DB();
extern void ITR_NMI();
extern void ITR_BP();
extern void ITR_OF();
extern void ITR_BR();
extern void ITR_UD();
extern void ITR_NM();
extern void ITR_DF();
extern void ITR_CSO();
extern void ITR_TS();
extern void ITR_NP();
extern void ITR_SS();
extern void ITR_GP();
extern void ITR_PF();
extern void ITR_RES1();
extern void ITR_MF();
extern void ITR_AC();
extern void ITR_MC();
extern void ITR_XF();
extern void ITR_RES2();

#endif
