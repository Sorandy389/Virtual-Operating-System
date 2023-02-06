/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

/* Reference:
 * https://wiki.osdev.org/Interrupts
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "idt.h"

#define IRQ_KEYBOARD		0x01	// the IRQ of keyboard on master PIC
#define KEYBOARD_DATA		0x60	// the data port of keyboard
#define KEYBOARD_COMMAND	0x64	// the command port of keyboard

#define KEYBOARD_RELEASE	0x80	// the offset of a button once being released

#define BACKSPACE			0x0E	// the scancode of BACKSPACE
#define TAB					0x0F	// the scancode of TAB
#define ENTER				0x1C	// the scancode of ENTER
#define CTRL				0x1D	// the scancode of CTRL
#define SHIFT_L				0x2A	// the scancode of LEFT SHIFT
#define SHIFT_R				0x36	// the scancode of RIGHT SHIFT
#define ALT					0x38	// the scancode of ALT
#define CAPSLOCK			0x3A	// the scancode of CAPSLOCK
#define UP_ARROW			0x48	// the scancode of UP_ARROW
#define LEFT_ARROW			0x4B	// the scancode of LEFT_ARROW
#define RIGHT_ARROW			0x4D	// the scancode of RIGHT_ARROW
#define DOWN_ARROW			0x4F	// the scancode of DOWN_ARROW
#define F1					0x3B	// the scancode of F1
#define F2					0x3C	// the scancode of F2
#define F3					0x3D	// the scancode of F3

#define CTRL_RELEASE		CTRL + KEYBOARD_RELEASE		// add the offset once being released
#define SHIFT_L_RELEASE		SHIFT_L + KEYBOARD_RELEASE	// add the offset once being released
#define SHIFT_R_RELEASE		SHIFT_R + KEYBOARD_RELEASE	// add the offset once being released
#define ALT_RELEASE			ALT + KEYBOARD_RELEASE		// add the offset once being released


/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard device */
void keyboard_handler(void);

/* Print the transformed character according to the input scancode */
void keyboard_print(uint8_t scancode);

#endif /*_KEYBOARD_H*/
