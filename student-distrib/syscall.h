#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "x86_desc.h"

#include "terminal.h"
#include "filesystem.h"
#include "rtc.h"
#include "page.h"

#define EIGHT_MB    0x800000      // 8mb
#define FOUR_MB     0x400000      // 4mb
#define EIGHT_KB    0x2000        // 8kb
#define _132_MB		0x8400000		//132mb

#define SYSCALL_N   10    // 10 syscalls
#define FD_N        8     // 8 file descriptors
#define FD_STDIN    0     // fd for stdin
#define FD_STDOUT   1     // fd for stdout
#define PROGRAM_IMAGE_ADDR 0x8048000  // program image address
#define PDE_INDEX	  PROGRAM_IMAGE_ADDR/FOUR_MB  // Page Directory Index for Program
#define PCB_MASK    0xFFFFE000    // PCB mask
#define FILE_TYPE_RTC  0          // rtc file type
#define FILE_TYPE_DIR  1          // directory file type
#define FILE_TYPE_FILE 2          // file file type
#define FILE_TYPE_TERM 3          // terminal file type
#define FILE_TYPE_N    4          // number of file types
#define ENTRY_START 24            // starting point of entry
#define MN_L        4             // length of magic number array
#define BYTE        4             // byte size

#define PROCESS_NUM_MAX 6		// max value of process number

// Magic numbers for checking exe file
const static uint8_t exe_magic_number[MN_L] = {0x7f, 0x45, 0x4c, 0x46};

// struct for file operation table
typedef struct {
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
} fops_t;

// File operation table
const static fops_t fops_table[FILE_TYPE_N] =
  {{rtc_read, rtc_write, rtc_open, rtc_close},
  {dir_read, dir_write, dir_open, dir_close},
  {file_read, file_write, file_open, file_close},
  {terminal_read, terminal_write, terminal_open, terminal_close}};

//struct for file descriptor
typedef struct {
  fops_t* fops;
  int32_t inode;
  int32_t file_pos;
  int32_t flags;
} file_desc_t;

//struct for pcb
typedef struct {
  file_desc_t file_desc_array[FD_N];
  int32_t esp_prev;
  int32_t ebp_prev;
  int32_t process_num_prev;
  int32_t esp;
  int32_t ebp;
  int32_t process_num;
  uint8_t argument[KEYBOARD_BUF_SIZE];
  int32_t terminal_num;
} pcb_t;

//status for six process number
int8_t process[PROCESS_NUM_MAX];

//number of active process number
int32_t current_process_num;

void flush_tlb();
void init_syscall();
int32_t halt (uint32_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
// int32_t set_handler (int32_t signum, void* handler_address);
// int32_t sigreturn (void);

#endif
