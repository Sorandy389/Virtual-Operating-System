#include "page.h"
#include "types.h"

/* void page_init();
 * Inputs: none
 * Return Value: none
 * Function: Initializes Paging */
void page_init() {
    int i = 0;
    //iterate all entries in PageDirectory and PageTable
    for(i = 0; i < NUM_ENTRIES; i++) {
      //first make all entries in PageDirectory bot readable and writable
      PageDirectory[i] = RW_ENABLE;
      //then put 10-bit address of physical memory in first 4MB into PTEs
      PageTable[i] = (i << ADDRESS_OFFSET);
      //make them both readable and writable as well
      PageTable[i] |= RW_ENABLE;
    }

    //set the PTE that points to video memory to writeable and mark it as present
    PageTable[VIDEO_MEM >> ADDRESS_OFFSET] |= (RW_ENABLE | PRESENT);
    //set the first PDE to point to PageTable and
    //set it to writeable and mark it as present
    //it is first because video memory resides in first 4MB
    PageDirectory[0] = ((unsigned int) PageTable) | (RW_ENABLE | PRESENT);
    //let second PDE point directly to 4MB-8MB address and
    //set it to writeable and mark it as present
    //it is second because kernel resides in 4MB-8MB and both virtual and phsical address are identical
    PageDirectory[1] = FOURMB_BINARY | FOURMB_FLAG | (RW_ENABLE | PRESENT);
    asm volatile(
          "movl %0, %%eax;"
          "movl %%eax, %%cr3;"
          "movl %%cr4, %%eax;"
          "orl $0x00000010, %%eax;"
          "movl %%eax, %%cr4;"
          "movl %%cr0, %%eax;"
          "orl $0x80000000, %%eax;"
          "movl %%eax, %%cr0;"
          :
          :"r"(PageDirectory)
          :"%eax"


    );
}
