#include "pit.h"
#include "syscall.h"

#define PIT_HZ          20   // frequency of PIT
#define PIT_MASK        0xFF // mask of PIT
#define PIT_RATE        1193180 / PIT_HZ // rate of PIT

#define IRQ_PIT         0x00  // irq of PIT
#define CH0_PORT   			0x40	// the port used for channel 0
#define CH1_PORT  			0X41	// the port used for channel 1
#define CH2_PORT 			  0x42	// the port used for channel 2
#define CMD_PORT 			  0x43	// the port used for mode/command


int32_t pcb_esp[7];
int32_t pcb_ebp[7];

/* pit_init
 * Description: Initialize the pit.
 * Input: none
 * Output: none
 */
void pit_init(void) {
  outb(0x36, CMD_PORT);
  outb(PIT_RATE & PIT_MASK, CH0_PORT);
  outb(PIT_RATE >> 8, CH0_PORT);
  add_irq(IRQ_PIT, pit_handler);
}

/* pit_handler
 * Description: handles pit interrupt calls
 * Input: none
 * Output: none
 */
void pit_handler(void) {
  send_eoi(IRQ_PIT);

  cli();

  // test_interrupts();
  if (!terminal_t[1].flag && !terminal_t[2].flag) {
	  sti();
	  return;
  }
  // Context switch

  terminal_num = (terminal_num + 1) % 3;
  while(!terminal_t[terminal_num].flag) {
    terminal_num = (terminal_num + 1) % 3;
  }

  int next_process = terminal_t[terminal_num].process_num;
  PageDirectory[PDE_INDEX] = (EIGHT_MB + ((next_process-1) * FOUR_MB)) | FOURMB_FLAG | (RW_ENABLE | PRESENT) | USER_ENABLE;

  if (terminal_num == terminal_displayed) {
    PageDirectory[_132_MB/FOUR_MB] = ((uint32_t)UserPageTable) | (RW_ENABLE | PRESENT) | USER_ENABLE;
    UserPageTable[0] = VIDEO | (RW_ENABLE | PRESENT) | USER_ENABLE;
  } else {
    PageDirectory[_132_MB/FOUR_MB] = ((uint32_t)VideoPageTable) | (RW_ENABLE | PRESENT) | USER_ENABLE;
    VideoPageTable[0] = ((int32_t) terminal_t[terminal_num].video_memory) | (RW_ENABLE | PRESENT) | USER_ENABLE;
  }
  flush_tlb();
  pcb_t * pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
  asm volatile(
    "movl %%esp, %%eax;"
    "movl %%ebp, %%ebx;"
    : "=a"(pcb_ptr->esp), "=b"(pcb_ptr->ebp)
    :
  );
  
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - EIGHT_KB * (next_process-1) - BYTE;
  pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
  /*
  if ((pcb_esp[pcb_ptr->process_num] != pcb_ptr->esp) || (pcb_ebp[pcb_ptr->process_num] != pcb_ptr->ebp)) {
	pcb_esp[pcb_ptr->process_num] = pcb_ptr->esp;
	pcb_ebp[pcb_ptr->process_num] = pcb_ptr->ebp;
	printf("before: %d, ", pcb_ptr->process_num);
	printf("esp: %d, ", pcb_ptr->esp);
	printf("ebp: %d\n", pcb_ptr->ebp);
}*/
  asm volatile(
    "movl %%eax, %%esp;"
    "movl %%ebx, %%ebp;"
    :
    : "a"(pcb_ptr->esp), "b"(pcb_ptr->ebp)
  );

  sti();
}
