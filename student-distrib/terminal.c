/* terminal.c - Functions to interact with the terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"

/* Open the terminal, do nothing */
int32_t terminal_open(const uint8_t* filename) {
	return 0;
}

/* Close the terminal, do nothing */
int32_t terminal_close(int32_t fd) {
	return 0;
}

/* terminal_read
 *
 * Store the keyboard buffer to the input address of buf
 * Inputs: buf, nbytes
 * Outputs: the number of bytes read
 * Side Effects: Store a line from the keyboard buffer to the buf
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	

	/* Wait until there is a line in the keyboard buffer */
	while (!terminal_t[terminal_num].buf_flag) {}

	/* Disable the interrupts */
	cli();

	int i, j;				// iterators
	int byte_read = 0;		// the number of bytes read
	uint8_t * char_buf = (uint8_t *) buf;
	/* read from the keyboard buffer until reach '\n' */
	for (i = 0; i < nbytes; i++) {
		char_buf[i] = terminal_t[terminal_num].key_buf[i];
		byte_read++;
		if (char_buf[i] == '\n')
			break;
	}

	/* Left shift the keyboard buffer to clean the line just read */
	for (j = 0; j < KEYBOARD_BUF_SIZE - byte_read; j++) {
		terminal_t[terminal_num].key_buf[j] = terminal_t[terminal_num].key_buf[j + byte_read];
	}
	for (j = 1; j <= byte_read; j++) {
		terminal_t[terminal_num].key_buf[KEYBOARD_BUF_SIZE - j] = '\0';
	}

	/* Decrease the number of bytes in the keyboard buffer */
	terminal_t[terminal_num].num_buf -= byte_read;
	/* Decrease the number of new lines */
	terminal_t[terminal_num].buf_flag--;

	/* Enable the interrupts */
	sti();

	/* Return the number of bytes read */
	return byte_read;


}

/* Output the chars stored in the input buffer
 * Should not change the keyboard buffer
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	int i;		// iterator
	/* Output the input buffer */
	cli();
	uint8_t * char_buf = (uint8_t *) buf;
	for (i = 0; i < nbytes; i++) {
		putc((uint8_t)char_buf[i]);
	}
	sti();
	/* Return the number of bytes output */
	return i;
}
