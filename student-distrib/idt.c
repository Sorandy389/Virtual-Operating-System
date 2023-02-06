#include "x86_desc.h"
#include "idt.h"
#include "handler_in_asbly.h"
#include "syscall.h"

/* void EXC_DE(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles divide exception. */
void EXC_DE(void){
    clear();
    printf("Divide Error");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_DB(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles reserved. */
void EXC_DB(void){
    clear();
    printf("Reserved");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_NMI(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles NMI interrupt. */
void EXC_NMI(void){
    clear();
    printf("NMI");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_BP(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles breakpoint exception. */
void EXC_BP(void){
    clear();
    printf("Breakpoint");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_OF(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles overflow exception. */
void EXC_OF(void){
    clear();
    printf("Overflow");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_BR(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles BOUND range exception. */
void EXC_BR(void){
    clear();
    printf("BOUND Range Exceeded");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_UD(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles invalid opcode exception. */
void EXC_UD(void){
    clear();
    printf("Undefined Opcode");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_NM(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles device not available exception. */
void EXC_NM(void){
    clear();
    printf("No Math Coprocessor");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_DF(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles double fault exception. */
void EXC_DF(void){
    clear();
    printf("Double Fault");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_CSO(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles Coprocessor Segment Overrun. */
void EXC_CSO(void){
    clear();
    printf("Coprocessor Segment Overrun");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_TS(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles invalid TSS exception. */
void EXC_TS(void){
    clear();
    printf("Invalid TSS");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_NP(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles segment not present. */
void EXC_NP(void){
    clear();
    printf("Segment Not Present");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_SS(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles stack fault exception. */
void EXC_SS(void){
    clear();
    printf("Stack-Segment Fault");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_GP(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles general protection exception. */
void EXC_GP(void){
    clear();
    printf("General Protection");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_PF(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles page fault exception. */
void EXC_PF(void){
    //clear();
    printf("Page Fault\n");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_RES1(void);
 * Inputs: void
 * Return Value: none
 * Function: Reserved handler. */
void EXC_RES1(void){
    clear();
    printf("reserved 1");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_MF(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles math fault. */
void EXC_MF(void){
    clear();
    printf("Math Fault");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_AC(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles alignment check exception. */
void EXC_AC(void){
    clear();
    printf("Alignment Check");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_MC(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles machine check exception. */
void EXC_MC(void){
    clear();
    printf("Machine Check");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_XF(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles floating point exception. */
void EXC_XF(void){
    clear();
    printf("Floating-Point Exception");
	//input 256 if program dies with an exception
	halt(256);
}

/* void EXC_RES2(void);
 * Inputs: void
 * Return Value: none
 * Function: Reserved handler. */
void EXC_RES2(void){
    clear();
    printf("reserved 2");
	//input 256 if program dies with an exception
	halt(256);
}

/* void idt_init();
 * Inputs: none
 * Return Value: none
 * Function: Initializes IDT */
void idt_init(){
    int i;
    for(i = 0; i < NUM_VEC; i++){
        //start with general descriptor seetings of exception
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 1;
        if(i >= 32){
            //if general interrupts, change reserved3
            idt[i].reserved3 = 0;
        }
        if(i == IDT_SYSCALL){
            //if system call, change privilege to 3
            idt[i].dpl = 3;
            idt[i].reserved3 = 1;
        }
    }
    SET_IDT_ENTRY(idt[0], ITR_DE);
    SET_IDT_ENTRY(idt[1], ITR_DB);
    SET_IDT_ENTRY(idt[2], ITR_NMI);
    SET_IDT_ENTRY(idt[3], ITR_BP);
    SET_IDT_ENTRY(idt[4], ITR_OF);
    SET_IDT_ENTRY(idt[5], ITR_BR);
    SET_IDT_ENTRY(idt[6], ITR_UD);
    SET_IDT_ENTRY(idt[7], ITR_NM);
    SET_IDT_ENTRY(idt[8], ITR_DF);
    SET_IDT_ENTRY(idt[9], ITR_CSO);
    SET_IDT_ENTRY(idt[10], ITR_TS);
    SET_IDT_ENTRY(idt[11], ITR_NP);
    SET_IDT_ENTRY(idt[12], ITR_SS);
    SET_IDT_ENTRY(idt[13], ITR_GP);
    SET_IDT_ENTRY(idt[14], ITR_PF);
    SET_IDT_ENTRY(idt[15], ITR_RES1);
    SET_IDT_ENTRY(idt[16], ITR_MF);
    SET_IDT_ENTRY(idt[17], ITR_AC);
    SET_IDT_ENTRY(idt[18], ITR_MC);
    SET_IDT_ENTRY(idt[19], ITR_XF);

    for(i = 20; i < 32; i++){
        SET_IDT_ENTRY(idt[i], EXC_RES2);
    }

    /* Initializes irq */
    SET_IDT_ENTRY(idt[0x20], ITR_IRQ0);
    SET_IDT_ENTRY(idt[0x21], ITR_IRQ1);
    SET_IDT_ENTRY(idt[0x22], ITR_IRQ2);
    SET_IDT_ENTRY(idt[0x23], ITR_IRQ3);
    SET_IDT_ENTRY(idt[0x24], ITR_IRQ4);
    SET_IDT_ENTRY(idt[0x25], ITR_IRQ5);
    SET_IDT_ENTRY(idt[0x26], ITR_IRQ6);
    SET_IDT_ENTRY(idt[0x27], ITR_IRQ7);
    SET_IDT_ENTRY(idt[0x28], ITR_IRQ8);
    SET_IDT_ENTRY(idt[0x29], ITR_IRQ9);
    SET_IDT_ENTRY(idt[0x2A], ITR_IRQA);
    SET_IDT_ENTRY(idt[0x2B], ITR_IRQB);
    SET_IDT_ENTRY(idt[0x2C], ITR_IRQC);
    SET_IDT_ENTRY(idt[0x2D], ITR_IRQD);
    SET_IDT_ENTRY(idt[0x2E], ITR_IRQE);
    SET_IDT_ENTRY(idt[0x2F], ITR_IRQF);

    /* Initializes syscall */
    SET_IDT_ENTRY(idt[0x80], ITR_SYS);
    init_syscall();

    lidt(idt_desc_ptr);
}

/* add_irq
 * Description: Add device to irq.
 * Input: irq_num - number of IRQ, handler - handler function
 * Output: None
 * Side Effect: Changes irq handler jumptable
 */
void add_irq(int irq_num, void (*handler)) {
    irq_handler_jumptable[irq_num] = handler;
    enable_irq(irq_num);
}
