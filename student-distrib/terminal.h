/* terminal.h - Defines used in interactions with the terminal
 * vim:ts=4 noexpandtab
 */


#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"
#include "keyboard.h"

/* Open the terminal */
int32_t terminal_open(const uint8_t* filename);
/* Close the terminal */
int32_t terminal_close(int32_t fd);
/* Store the keyboard buffer to the input address of buf */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Output the chars stored in the input buffer */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif /*_TERMINAL_H*/
