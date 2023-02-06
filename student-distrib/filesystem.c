#include "filesystem.h"
#include "lib.h"
#include "types.h"
#include "syscall.h"

//put make starting address of file system GLOBAL
static uint32_t filesystem_start;
//a pointer to first boot_block
static boot_block_t* boot_block;
//the index of directory
static int dir_index;

/*
	function name: filesystem_init
	input: start address from kernel.c
	output: none
	purpose: initialize file system, make starting address global
	and initialize boot_block
*/
void filesystem_init(uint32_t address) {
	filesystem_start = address;
	boot_block = (boot_block_t*) address;
	to_test = boot_block;
	dir_index = 0;
}

/*	------------------- File Functions -------------------- */

/* file_read
 * Inputs: fd - file descriptor, buffer - buffer to store, nbytes - size limit
 * Return Value: number of bytes copied
 * Description: Copies nbytes of file into buffer and update the file_read position
 * in file descriptor array for mp3.3
 */
int32_t file_read(int32_t fd, void * buffer, int32_t nbytes) {
	//get inode from file descriptor array
	pcb_t * cur_pcb = (pcb_t *) (tss.esp0 & PCB_MASK);
	uint32_t cur_inode = cur_pcb->file_desc_array[fd].inode;
	//get starting read position of file from file descriptor array as offset
	uint32_t cur_offset = cur_pcb->file_desc_array[fd].file_pos;
	//use read date to copy date into buffer
	int read_bytes = read_data(cur_inode,cur_offset, buffer, nbytes);
	//update file position in file drescriptor array
	cur_pcb->file_desc_array[fd].file_pos  +=  read_bytes;
	//return bytes read
	return read_bytes;
}

/* file_write
 * Inputs: fd - file descriptor, buffer - buffer to write, nbytes - size limit
 * Return Value: -1 (temp)
 * Description: File wrote call: do nothing
 */
int32_t file_write(int32_t fd, const void * buffer, int32_t nbytes) {
	return -1;
}

/* file_open
 * Inputs: filename
 * Return Value: 0
 * Description: File open call: do nothing
 */
int32_t file_open(const uint8_t* filename) {
	return 0;
}

/* file_close
 * Inputs: fd - file descriptor
 * Return Value: 0
 * Description: File close call: do nothing
 */
int32_t file_close(int32_t fd) {
	return 0;
}

/*	------------------- Directory Functions -------------------- */

/* dir_read
 * Inputs: fd - file descriptor, buffer - buffer to store filename, nbytes - number of bytes
 * Return Value: number of bytes read
 * Description: list all files in directory */
int32_t dir_read(int32_t fd, void * buf, int32_t nbytes) {
	// If dir_index reaches the end, dir_read returns 0
	if (dir_index >= to_test->dir_count) {
		return 0;
	}
	// Find and return the next file name
	dentry_t cur_dentry;
	if (read_dentry_by_index(dir_index, &cur_dentry)) return -1;
	int cur_length = strlen((int8_t*)cur_dentry.file_name);
	cur_length = MIN(cur_length, FILENAME_LEN);
	cur_length = MIN(cur_length, nbytes);
	strncpy(buf, (int8_t*)cur_dentry.file_name, cur_length);
	dir_index++;

	return cur_length;
}

/* dir_write
 * Inputs: none
 * Return Value: -1 (temp)
 * Description: Directory write call: do nothing */
int32_t dir_write(int32_t fd, const void * buf, int32_t nbytes) {
	return -1;
}

/* dir_open
 * Inputs: fname - file name
 * Return Value: 0 (temp)
 * Description: Open directory files by filename */
int32_t dir_open(const uint8_t* filename) {
	dir_index = 0;
	return 0;
}

/* dir_close
 * Inputs: none
 * Return Value: 0 (temp)
 * Description: Close call: do nothing */
int32_t dir_close(int32_t fd) {
	return 0;
}

/*	------------------- File System Functions -------------------- */

/*
	function name: read_dentry_by_name
	input: fname = file name, dentry = a dir entry temporay variable
	output: -1 for failure and 0 for success
	purpose: first compare file name to each of file names of dentries,
	find the right dentry and deep copy it into dentry through parameter.
	It uses helper function read_dentry_by_index
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
	int fname_length =strlen((int8_t*)fname), cur_length = 0;
	int i = 0;
	//check for invalid dentry and invalide length of filename
	//return failure if condition is not met
	if(fname_length <= 0 || fname_length > FILENAME_LEN || dentry == NULL) {
		return -1;
	}
	//iterate each dentry in boot_block
	for(i = 0; i < boot_block -> dir_count; i++) {
		//declare local dentry
		dentry_t cur_dentry;
		//call helper function to copy dentry into local one
		read_dentry_by_index(i, &cur_dentry);
		//calculate local dentry length
		cur_length = strlen((int8_t*)(cur_dentry.file_name));
		//condition 1: compare string of local name to file name, up to file name length
		//condition 2: test if two strings are equal or file name length is FILENAME_LEN
		//this is for cases when file name greater than 32 and we only need first 32 characters
		if(strncmp((int8_t*)fname, (int8_t*)cur_dentry.file_name, fname_length) == 0 && (
			fname_length == cur_length||fname_length == FILENAME_LEN)) {
			//if true copy contents of local dentry to dentry pointed in parameter
			memcpy(dentry, &cur_dentry, sizeof(dentry_t));
			//return success
			return 0;
		}
	}
	//if iterated all and still finds none, return failure
	return -1;

}

/*
	function name: read_dentry_by_index
	input: index = index into direntries in boot block, dentry = a dir entry temporay variable
	output: -1 for failure and 0 for success
	purpose: it is used to, if index is valid, deep copy dentry in boot block
	into dentry pointed by second parameter
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
	//error checking if index is valid
	if(index < 0 || index >= boot_block -> dir_count) {
		//return failure
		return -1;
	}
	//performs deep copy from dentry in boot block
	//into dentry pointed by second parameter
	memcpy(dentry, &(boot_block->direntries[index]), sizeof(dentry_t));
	//return success
	return 0;

}

/*
	function name: read_data
	input: inode = index of inode in memory,
		   offset = offset from start of file(number of bytes),
		   buf = buffer to copy to,
		   length = length of bytes to copy
	output: -1 for failure and 0 for end of file and i for number of successful copy
	purpose: it is used to copy data pointed by data array in inode and offset,
			to buf array, specified by length
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	//starting address of inode block
	uint32_t inode_start = filesystem_start + BLOCK_SIZE_IN_BYTES;
	//starting address of data block
	uint32_t data_start = filesystem_start + BLOCK_SIZE_IN_BYTES + (boot_block -> inode_count * BLOCK_SIZE_IN_BYTES);
	//pointer to current inode
	inode_t* cur_inode =(inode_t*) (inode_start + inode * BLOCK_SIZE_IN_BYTES);
	//file langth in number of bytes
	uint32_t cur_length = cur_inode->length;
	//test if number of bytes remaining in file or length is smaller,
	//to make sure there is no ove copy beyond end of file
	uint32_t copy_length = MIN((cur_length - offset), length);
	uint32_t i = 0;
	uint32_t data_block_index;
	uint8_t* data_block_address;
	//test if inode is correct index
	if(inode < 0 || inode >= (boot_block -> inode_count)) {
		return -1;
	}
	//test if it is end of file
	if(offset >= cur_length) {
		return 0;
	}
	//iteration to perform copy to buf
	for(i = 0; i < copy_length; i++) {
		//calculate block index
		data_block_index = (offset + i) / BLOCK_SIZE_IN_BYTES;
		//calculate specific data byte index
		data_block_address = (uint8_t*)(data_start + (cur_inode->data_block_num[data_block_index] * BLOCK_SIZE_IN_BYTES));
		//perform copy
		buf[i] = data_block_address[(offset + i) % BLOCK_SIZE_IN_BYTES];
	}
	//return number of successful copies
	return i;
}
