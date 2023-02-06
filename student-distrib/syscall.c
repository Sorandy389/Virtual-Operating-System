#include "syscall.h"
#include "handler_in_asbly.h"
#include "lib.h"

void init_syscall() {
  syscall_jumptable[0] = 0x0;
  syscall_jumptable[1] = &halt;
  syscall_jumptable[2] = &execute;
  syscall_jumptable[3] = &read;
  syscall_jumptable[4] = &write;
  syscall_jumptable[5] = &open;
  syscall_jumptable[6] = &close;
  syscall_jumptable[7] = &getargs;
  syscall_jumptable[8] = &vidmap;

  process[0] = 1;
}

/*
	function: halt
	inputs: status
	output: function success(0)/failure(-1)
	description:
		this function prepares and halts .exe progrom in order of:
		1.Restore Parent Data
		2.Restore Parent Paging
		3.Clean FD
		4.Update TSS
		5.Jump to execute return
*/
int32_t halt (uint32_t status) {

	current_process_num = terminal_t[terminal_num].process_num;
	// Set current process free
	process[current_process_num] = 0;

	terminal_t[terminal_num].total_process--;
	if (!terminal_t[terminal_num].total_process) {
		terminal_t[terminal_num].process_num = 0;
		execute((uint8_t*)"shell");
	}


	/* Step 1: Restore Parent Data */
	pcb_t * pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
	// pcb_t * pcb_ptr = (pcb_t *)(EIGHT_MB - EIGHT_KB * current_process_num);
	terminal_t[terminal_num].process_num = pcb_ptr->process_num_prev;

	// Check if there is no more shell to exit
/*	int i, total_process_num;
	total_process_num = 0;
	for (i = 0; i < PROCESS_NUM_MAX; i++) {
		total_process_num += process[i];
	}
	if (total_process_num == 1) {
		terminal_t[terminal_num].process_num = 0;
		execute((uint8_t*)"shell");
	}*/

	/* Step 2: Restore Parent Paging */
	PageDirectory[PDE_INDEX] = (EIGHT_MB + (terminal_t[terminal_num].process_num - 1) * FOUR_MB) | FOURMB_FLAG | (RW_ENABLE | PRESENT) | USER_ENABLE;
	flush_tlb();

	/* Step 3: Clean FD */
	int i;
	for (i = FD_STDOUT+1; i < FD_N; i++) {
		if (pcb_ptr->file_desc_array[i].flags == 1) {
			close(i);
		}
	}

	/* Step 4: TSS */
	/* Update TSS */
	//tss.esp0 = pcb_ptr->esp_prev;
	tss.esp0 = EIGHT_MB - EIGHT_KB * (pcb_ptr->process_num_prev-1) - BYTE;

	/* Jump to execute return */
	asm volatile("				\n\
		movl %0, %%eax			\n\
		movl %1, %%esp			\n\
		movl %2, %%ebp			\n\
		leave             	\n\
		ret"
		:
		: "r"((uint32_t)status), "r"(pcb_ptr->esp_prev), "r"(pcb_ptr->ebp_prev)
		: "%eax"
	);

	return 0;
}

// https://wiki.osdev.org/TLB
void flush_tlb() {
	asm volatile ("  			\n\
		movl %%cr3, %%eax	 	\n\
		movl %%eax, %%cr3"
    :
    :
    : "eax", "memory", "cc"
    );
}

/*
	function: execute
	inputs: command, containing input string
	output: function success(0)/failure(-1)
	description:
		this function prepares and executes .exe progrom in order of:
		1.parsing
		2..exe file check
		3.paging to convert 128mb virtual address to 8mb physical address
		4. load into physical memory
		5. create pcb
		6. context switch
*/
int32_t execute (const uint8_t* command) {

  current_process_num = terminal_t[terminal_num].process_num;
  if (command == NULL) return -1;
  // set limitation of process number

  // find the next available process
  int i, next_process_num;;
  next_process_num = -1;
  for (i = 1; i <= PROCESS_NUM_MAX; i++) {
    if (process[i] == 0) {
		next_process_num = i;
		break;
	}
  }
  if (next_process_num == -1) {
    printf("no more process\n");
    return 0;
  }

  //clear IF flag
  cli();
  dentry_t exe_dentry;
  uint8_t exe_header[MN_L];
  uint32_t entry;
  uint8_t cmd[KEYBOARD_BUF_SIZE], argument[KEYBOARD_BUF_SIZE];

  //------------------------Parsing------------------------
  int start, end;
  start = end = 0;
  //eliminate heading spaces
  while(command[start] == ' ')
	  start++;
  end = start;
  //move end index toward end of filename
  while(command[end] != ' ' && command[end] != '\n' && command[end] != '\0') {
    end++;
  }
  //perform string copy
  strncpy((char*)cmd, (char*)command + start, end - start);
  //add null terminated char
  cmd[end - start] = '\0';

  //parsing argument
  start = end;
  //eliminate heading spaces
  while(command[start] == ' ')
	  start++;
  end = start;
  //move end index toward end of filename
  while(command[end] != ' ' && command[end] != '\n' && command[end] != '\0') {
    end++;
  }
  //perform string copy
  strncpy((char*)argument, (char*)command + start, end - start);
  //add null terminated char
  argument[end - start] = '\0';

  //------------------------Check magic number------------------------
  //check if filename is valid
  if (read_dentry_by_name(cmd, &exe_dentry)) return -1;
  //check first for characters
  read_data(exe_dentry.inode_num, 0, exe_header, MN_L);
  for (i = 0; i < MN_L; i++) {
	  //if any one does not match return failure
    if (exe_header[i] != exe_magic_number[i]) return -1;
  }
  //get entry
  read_data(exe_dentry.inode_num, ENTRY_START, (uint8_t*) &entry, MN_L);
  //------------------------Paging------------------------
  int32_t prog_addr = PROGRAM_IMAGE_ADDR;
  PageDirectory[PDE_INDEX] = (EIGHT_MB + (next_process_num - 1) * FOUR_MB) | FOURMB_FLAG | (RW_ENABLE | PRESENT) | USER_ENABLE;
  flush_tlb();
  //------------------------Load------------------------
  read_data(exe_dentry.inode_num, 0, (uint8_t*) prog_addr, FOUR_MB);
  // printf("Bro we got the paging and loading done\n");
  //------------------------PCB------------------------
  process[next_process_num] = 1;
  terminal_t[terminal_num].flag = 1;
  pcb_t * pcb_ptr = (pcb_t *)(EIGHT_MB - EIGHT_KB * next_process_num);
  // store argument in pcb
  strcpy((char*)pcb_ptr->argument, (char*)argument);

  // Initialize stdin/stdout
  pcb_ptr->file_desc_array[0].fops = (fops_t*) fops_table + FILE_TYPE_TERM;
  pcb_ptr->file_desc_array[0].flags = 1;
  pcb_ptr->file_desc_array[1].fops = (fops_t*) fops_table + FILE_TYPE_TERM;
  pcb_ptr->file_desc_array[1].flags = 1;

  // CS
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - EIGHT_KB * (next_process_num - 1) - BYTE;
  pcb_ptr->process_num_prev = current_process_num;
  pcb_ptr->process_num = next_process_num;
  terminal_t[terminal_num].process_num = next_process_num;
  terminal_t[terminal_num].total_process++;

  //------------------------context switch------------------------
  // Save parent esp/ebp
  asm volatile("      \n\
	movl %%esp, %%eax		\n\
	movl %%ebp, %%ebx"
	: "=a"(pcb_ptr->esp_prev), "=b"(pcb_ptr->ebp_prev)
  );
	// set IF flag
  sti();
  asm volatile("cli       \n\
    movw  $0x2B, %%ax     \n\
    movw  %%ax, %%ds      \n\
    pushl $0x2B           \n\
    pushl $0x83FFFFC      \n\
    pushfl                \n\
    popl %%eax            \n\
    orl $0x4200, %%eax    \n\
    pushl %%eax           \n\
    pushl $0x23           \n\
    pushl %%ebx           \n\
    iret"
    :
    : "b"(entry)
    : "%eax"
  );
  return 0;
}

/*
	function: read
	inputs: fd - file descriptor, buf - buffer for read data, nbytes - number of bytes to read
	output: function success(0)/failure(-1)
	description:
		this function reads a file specified by number of bytes into buffer
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
	//initial check for valid inputs
  //read disabled for STDOUT
  if(fd < 0 || fd >= FD_N || buf == NULL || nbytes < 0 || fd == FD_STDOUT)
    return -1;
	//get pcb pointer
  pcb_t * pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
  if (pcb_ptr->file_desc_array[fd].flags == 0) {
	  return -1;
  }
  //call read syscall from file_desc_array
  return pcb_ptr->file_desc_array[fd].fops->read(fd,buf,nbytes);
}


/*
	function: write
	inputs: fd - file descriptor, buf - buffer for write data, nbytes - number of bytes to write
	output: function success(0)/failure(-1)
	description:
		this function writes a file specified by number of bytes from buffer
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
	//initial check for valid inputs
  //write disabled for STDIN
  if(fd < 0 || fd >= FD_N || buf == NULL || nbytes < 0 || fd == FD_STDIN)
    return -1;
	//get pcb pointer
  pcb_t * pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
    //call write syscall from file_desc_array
  return pcb_ptr->file_desc_array[fd].fops->write(fd,buf,nbytes);
}

/*
	function: open
	inputs: filename - name of file to open
	output: function success(0)/failure(-1)
	description:
		this function opens a file and initialize function calls according to
		file type
*/
int32_t open (const uint8_t* filename) {
	//initial check
  if (filename == NULL) return -1;
  int32_t i;
  dentry_t file_dentry;
  //get pcb pointer
  pcb_t *pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
  //check if filename is valid
  if (read_dentry_by_name(filename, &file_dentry) == -1) return -1;
  //check for next available file slot
  for (i = 2; i < FD_N; i++) {
    if (pcb_ptr->file_desc_array[i].flags == 0) {

      switch (file_dentry.file_type) {
		  //initialize proxy functions according to file type
        case FILE_TYPE_RTC:
          if (rtc_open(filename) != 0) return -1;
          pcb_ptr->file_desc_array[i].fops = (fops_t*) fops_table + FILE_TYPE_RTC;
          pcb_ptr->file_desc_array[i].inode = NULL;
          break;
        case FILE_TYPE_DIR:
          if (dir_open(filename) != 0) return -1;
          pcb_ptr->file_desc_array[i].fops = (fops_t*) fops_table + FILE_TYPE_DIR;
          pcb_ptr->file_desc_array[i].inode = NULL;
          break;
        case FILE_TYPE_FILE:
          if (file_open(filename) != 0) return -1;
          pcb_ptr->file_desc_array[i].fops = (fops_t*) fops_table + FILE_TYPE_FILE;
          pcb_ptr->file_desc_array[i].inode = file_dentry.inode_num;
          break;
      }
	  //set flag to in use
      pcb_ptr->file_desc_array[i].flags = 1;
	  //set starting reading position to 0
      pcb_ptr->file_desc_array[i].file_pos = 0;

      return i;
    }
  }
  return -1;
}

/*
	function: close
	inputs: fd - file descriptor
	output: function success(0)/failure(-1)
	description:
		this function closes a file
*/
int32_t close (int32_t fd) {
	// check for invalid file descriptor
  if (fd <= FD_STDOUT || fd >= FD_N) return -1;
	//get pcb pointer
  pcb_t *pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
	//return failure if closing a vacant file
  if (pcb_ptr->file_desc_array[fd].flags == 0){
    return -1;
  }
  //put this slot to not in use
  pcb_ptr->file_desc_array[fd].flags = 0;
	//call corresponding close syscall and return its close status
  if (pcb_ptr->file_desc_array[fd].fops->close(fd) != 0) {
    return -1;
  }
	// close success
  return 0;
}

/*
	function: getargs
	inputs: buf - buffer to store argument, nbytes - number of bytes to copy
	output: function success(0)/failure(-1)
	description:
		this function provides the argument from pcb for user
*/
int32_t getargs (uint8_t* buf, int32_t nbytes){
	if (buf == NULL || nbytes < 0) {
		return -1;
	}
	pcb_t *pcb_ptr = (pcb_t *) (tss.esp0 & PCB_MASK);
	if (pcb_ptr->argument[0] == '\0') {
		return -1;
	}
	strncpy((char*)buf, (char*)pcb_ptr->argument, nbytes);
	return 0;
}

/*
	function: vidmap
	inputs: screen_start - buffer to store pointer to the start of screen
	output: virtual address of the screen
	description:
		this function provides the screen start of video map for user
*/
int32_t vidmap (uint8_t** screen_start) {
	if (screen_start == NULL || screen_start > (uint8_t**)_132_MB || screen_start < (uint8_t**)(_132_MB - FOUR_MB)) {
		return -1;
	}
  cli();
	// paging remap
	int32_t pde_index = _132_MB/FOUR_MB;
	PageDirectory[pde_index] = ((uint32_t)UserPageTable) | (RW_ENABLE | PRESENT) | USER_ENABLE;
	UserPageTable[0] = VIDEO | (RW_ENABLE | PRESENT) | USER_ENABLE;
  flush_tlb();

	*screen_start = (uint8_t*)_132_MB;
  sti();
	return _132_MB;
}

// int32_t set_handler (int32_t signum, void* handler_address);
// int32_t sigreturn (void);
