#include "types.h"

#define NUM_ENTRIES 1024             //number of PDE and PTE per page
#define PAGE_SIZE_IN_BYTES 4096      //size of 4kB pages in bytes, used for alignment
#define RW_ENABLE 0x02               //second bit in 32-bit entry for enabling bot read and write
#define PRESENT 0x01                 //first bit in 32-bit entry for telling presence of address
#define USER_ENABLE 0x04             //user enable
#define ADDRESS_OFFSET 12            //address strats from bit 12 onwards
#define VIDEO_MEM 0xB8000            //starting address of video memory as specified in documents
#define FOURMB_BINARY 0x400000       //address of 4MB in hex
#define FOURMB_FLAG 0x80             //eighth bit specifying size of page the directory points to

uint32_t  PageTable[NUM_ENTRIES] __attribute__((aligned (PAGE_SIZE_IN_BYTES)));
uint32_t  PageDirectory[NUM_ENTRIES] __attribute__((aligned (PAGE_SIZE_IN_BYTES)));
uint32_t  UserPageTable[NUM_ENTRIES] __attribute__((aligned (PAGE_SIZE_IN_BYTES)));
uint32_t  VideoPageTable[NUM_ENTRIES] __attribute__((aligned (PAGE_SIZE_IN_BYTES)));

void page_init();
