#ifndef _FILESYSTEM_H
#define	_FILESYSTEM_H

#include "lib.h"
#include "types.h"

#define FILENAME_LEN			32
#define DENTRY_RESERVED_BYTES   24
#define BOOT_RESERVED_BYTES     52
#define NUM_DENTRY_IN_BOOT      63
#define BLOCKS_IN_INODE         1024
#define DATA_BLOCK_ARRAY_SIZE   4096
#define BLOCK_SIZE_IN_BYTES		4096
#define MIN(x, y) ((x < y) ? x : y)

//struct for dentry
typedef struct dentry {
    uint8_t   file_name[FILENAME_LEN];
    uint32_t  file_type;
    uint32_t  inode_num;
    uint8_t   reserved[DENTRY_RESERVED_BYTES];
} dentry_t;

//struct for boot_block
typedef struct boot_block {
    uint32_t  dir_count;
    uint32_t  inode_count;
    uint32_t  data_count;
    uint8_t   reserved[BOOT_RESERVED_BYTES];
    dentry_t  direntries[NUM_DENTRY_IN_BOOT];
} boot_block_t;

//struct for inode
typedef struct inode {
    uint32_t  length;
    uint32_t data_block_num[BLOCKS_IN_INODE - 1];
} inode_t;

//struct for data block
typedef struct data_block {
    uint8_t data_block_array[DATA_BLOCK_ARRAY_SIZE];
} data_block_t;

boot_block_t * to_test;

// file and directory calls
/* file_read
 * Inputs: fd - file descriptor, buffer - buffer to store, nbytes - size limit
 * Return Value: number of bytes copied
 * Description: Copies nbytes of file into buffer and update the file_read position
 * in file descriptor array for mp3.3
 */
int32_t file_read(int32_t fd, void * buffer, int32_t nbytes);

/* file_write
 * Inputs: fd - file descriptor, buffer - buffer to write, nbytes - size limit
 * Return Value: -1 (temp)
 * Description: File wrote call: do nothing
 */
int32_t file_write(int32_t fd, const void * buffer, int32_t nbytes);

/* file_open
 * Inputs: filename
 * Return Value: 0
 * Description: File open call: do nothing
 */
int32_t file_open(const uint8_t* filename);

/* file_close
 * Inputs: fd - file descriptor
 * Return Value: 0
 * Description: File close call: do nothing
 */
int32_t file_close(int32_t fd);

int32_t dir_read(int32_t fd, void * buffer, int32_t nbytes);
int32_t dir_write(int32_t fd, const void * buffer, int32_t nbytes);
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);

// header functions declared below
void filesystem_init(uint32_t address);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
