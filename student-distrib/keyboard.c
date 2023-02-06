/* keyboard.c - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

/* Reference: http://www.osdever.net/bkerndev/Docs/keyboard.htm */

#include "keyboard.h"
#include "syscall.h"

#define PRESS		1	// Set 1 for press
#define RELEASE		0	// Set 0 for release
#define TAB_SIZE	4

/* The status of buttons, pressed or released */
static int8_t CTRL_PRESS = RELEASE;
static int8_t CAPSLOCK_PRESS = RELEASE;
static int8_t SHIFT_PRESS = RELEASE;
static int8_t ALT_PRESS = RELEASE;

/* the map of keyboard
 * transforming the scanned code to the character on the keyboard
 */
unsigned char keyboard_original[128] =
{
     0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 0x09 */
   '9', '0', '-', '=', '\b',	/* Backspace 0x0E*/
   '\t',			/* Tab 0x0F*/
   'q', 'w', 'e', 'r',	/* 0x13 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key 0x1C*/
     0,			/* 0x1D   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 0x27 */
  '\'', '`',   0,		/* Left shift 0x2A*/
  '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 0x31 */
   'm', ',', '.', '/',   0,				/* Right shift 0x36*/
   '*',
     0,	/* Alt 0x38*/
   ' ',	/* Space bar */
     0,	/* Caps lock 0x3A*/
     0,	/* 59 - F1 key ... > 0x3B*/
     0,   0,   0,   0,   0,   0,   0,   0,
     0,	/* < ... F10 0x44*/
     0,	/* 69 - Num lock 0x45*/
     0,	/* Scroll Lock 0x46*/
     0,	/* Home key 0x47*/
     0,	/* Up Arrow 0x48*/
     0,	/* Page Up 0x49*/
   '-',
     0,	/* Left Arrow 0x4B*/
     0,
     0,	/* Right Arrow 0x4D*/
   '+',
     0,	/* 79 - End key 0x4F*/
     0,	/* Down Arrow 0x50*/
     0,	/* Page Down 0x51*/
     0,	/* Insert Key 0x52*/
     0,	/* Delete Key 0x53*/
     0,   0,   0,
     0,	/* F11 Key 0x57*/
     0,	/* F12 Key 0x58*/
     0,	/* All other keys are undefined 0x59*/
};

/* the map of keyboard, when CAPSLOCK is pressed
 * transforming the scanned code to the character on the keyboard
 */
unsigned char keyboard_capslock[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* the map of keyboard, when SHIFT is pressed
 * transforming the scanned code to the character on the keyboard
 */
unsigned char keyboard_shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '"', '~',   0,		/* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* the map of keyboard, when both CAPSLOCK and SHIFT is pressed
 * transforming the scanned code to the character on the keyboard
 */
unsigned char keyboard_capsshift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',	/* 39 */
 '"', '~',   0,		/* Left shift */
 '|', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};



/* Initialize the keyboard */
void keyboard_init(void) {
	/* Enable the irq for the keyboard */
	add_irq(IRQ_KEYBOARD, keyboard_handler);
}

/* keyboard_handler
 * Description: Get the data input for keyboard and handle it
 * Input: None
 * Output: None
 * Side Effect: Change the button statuses, output the char and store it in the keyboard buffer
 */
void keyboard_handler(void) {
	int i;
	/* Disable the interrupts */
	uint32_t flags;
	cli_and_save(flags);

 	/* Send EOI to the IRQ of the keyboard */
 	send_eoi(IRQ_KEYBOARD);

	/* Read the scanned code from keyboard data */
	int32_t scancode = inb(KEYBOARD_DATA);

	/* Check the input of keyboard */
	switch (scancode) {
		/* Set the status of CTRL to being pressed */
		case CTRL:
			CTRL_PRESS = PRESS;
			break;
		/* Set the status of SHIFT to being pressed */
		case SHIFT_L:
		case SHIFT_R:
			SHIFT_PRESS = PRESS;
			break;
		/* Set the status of ALT to being pressed */
		case ALT:
			ALT_PRESS = PRESS;
			break;
		/* Set the status of CAPSLOCK to being pressed */
		case CAPSLOCK:
			/* Exchange between being pressed and released */
			CAPSLOCK_PRESS = 1 - CAPSLOCK_PRESS;
			break;
		/* Set the status of CTRL to being released */
		case CTRL_RELEASE:
			CTRL_PRESS = RELEASE;
			break;
		/* Set the status of SHIFT to being released */
		case SHIFT_L_RELEASE:
		case SHIFT_R_RELEASE:
			SHIFT_PRESS = RELEASE;
			break;
		/* Set the status of ALT to being released */
		case ALT_RELEASE:
			ALT_PRESS = RELEASE;
			break;

		/* Switch among terminals */
		case F1:
			if (ALT_PRESS && (terminal_displayed != 0)) {
				switch_terminal(0);
			}
			break;
		case F2:
			if (ALT_PRESS && (terminal_displayed != 1)) {
				switch_terminal(1);
			}
			break;
		case F3:
			if (ALT_PRESS && (terminal_displayed != 2)) {
				switch_terminal(2);
			}
			break;
			// if (ALT_PRESS && (terminal_displayed != 2)) {
			// 	switch_terminal(2);
			// 	if (!terminal_t[1].flag) {
			// 		cli();
			// 		clear();
			// 		terminal_num = 2;
			// 		execute((uint8_t*)"shell");
			// 		sti();
			// 	}
			// }
			// break;

		/* If the pressed button is TAB */
		case TAB:
			for (i = 0; i < TAB_SIZE; i++) {
				putc2screen(' ');
				terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = ' ';
			}
			break;

		/* If the pressed button is backspace */
		case BACKSPACE:
			/* delete char from both the keyboard buffer and the screen */
			if (terminal_t[terminal_displayed].num_buf > 0) {
				putc2screen(keyboard_original[scancode]);
				terminal_t[terminal_displayed].num_buf--;
			}
			break;


		/* If the pressed button is enter */
		case ENTER:
			if (terminal_t[terminal_displayed].num_buf == KEYBOARD_BUF_SIZE) {
				break;
			}
			/* Store the '\n' in the keyboard buffer */
			terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = keyboard_original[scancode];
			/* Output the ENTER to the screen */
			putc2screen(keyboard_original[scancode]);

			/* a new line is entered */
			terminal_t[terminal_displayed].buf_flag++;
			break;

		default:
			/* Otherwise output the input char from the keyboard */
			keyboard_print((uint8_t)scancode);
			break;
	}

	/* Enable the interrupts */
	sti();
	restore_flags(flags);
}

/* keyboard_print
 * Description: Transform the input scanned code to characters.
 * Input: None
 * Output: None
 * Side Effect: Output the char and store it in the keyboard buffer
 */
void keyboard_print(uint8_t scancode) {
	/* Do nothing if the scancode beyond the keyboard map */
	if (scancode >= 0x60) return;
	/* Clean the screen and keyboard buffer if ctrl+l pressed */
	if (CTRL_PRESS && keyboard_original[scancode] == 'l') {
		int i;
		for (i = 0; i < KEYBOARD_BUF_SIZE; i++) {
			terminal_t[terminal_displayed].key_buf[i] = '\0';
		}
		terminal_t[terminal_displayed].num_buf = 0;
		terminal_t[terminal_displayed].buf_flag = 0;
		terminal_t[terminal_displayed].screen_x = 0;
		terminal_t[terminal_displayed].screen_y = 0;
		terminal_t[terminal_displayed].start_line_ptr = 0;
    cli();
    int temp = terminal_num;
    terminal_num = terminal_displayed;
    clear();
    terminal_num = temp;
    sti();
		return;
	}
	/* Do nothing if the buffer is full */
	if (terminal_t[terminal_displayed].num_buf >= KEYBOARD_BUF_SIZE - 1) {
		return;
	}

	/* When CAPSLOCK is pressed */
	if (CAPSLOCK_PRESS && !SHIFT_PRESS) {
		if (keyboard_capslock[scancode]) {
			/* Store the transformed character into the video memory */
			putc2screen(keyboard_capslock[scancode]);
			/* Store the char in the keyboard buffer*/
			terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = keyboard_capslock[scancode];
		}
	}
	/* When SHIFT is pressed */
	else if (!CAPSLOCK_PRESS && SHIFT_PRESS) {
		if (keyboard_shift[scancode]) {
			/* Store the transformed character into the video memory */
			putc2screen(keyboard_shift[scancode]);
			/* Store the char in the keyboard buffer*/
			terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = keyboard_shift[scancode];
		}
	}
	/* When both CAPSLOCK and SHIFT is pressed */
	else if (CAPSLOCK_PRESS && SHIFT_PRESS) {
		if (keyboard_capsshift[scancode]) {
			/* Store the transformed character into the video memory */
			putc2screen(keyboard_capsshift[scancode]);
			/* Store the char in the keyboard buffer*/
			terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = keyboard_capsshift[scancode];
		}
	}
	/* Otherwise use the original keyboard map */
	else {
		if (keyboard_original[scancode]) {
			/* Store the transformed character into the video memory */
			putc2screen(keyboard_original[scancode]);
			/* Store the char in the keyboard buffer*/
			terminal_t[terminal_displayed].key_buf[terminal_t[terminal_displayed].num_buf++] = keyboard_original[scancode];
		}
	}
}
