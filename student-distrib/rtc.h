/* rtc.h - Defines used in interactions with the rtc
 * vim:ts=4 noexpandtab
 */

/* Reference: https://wiki.osdev.org/RTC */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"

#define IRQ_RTC   				0x08	// the IRQ of rtc on slave PIC
#define RTC_PORT  				0X70	// the port used for RTC
#define CMOS_PORT 				0x71	// the port used for CMOS
#define RTCREG_A_DISABLE_NMI	0x8A	// the register B of rtc and setting 0x80 to disable NMI
#define RTCREG_B_DISABLE_NMI	0x8B	// the register B of rtc and setting 0x80 to disable NMI
#define RTCREG_C			 	0x0C	// the register C of rtc
#define MAX_RTC_RATE      9     // corresponding max frequency is 1024 Hz
#define MAX_RTC_FREQ	  1024  // max frequency
#define MASK_L            0xF0  // masks out the left 4 bits
#define MASK_R            0x0F  // masks out the right 4 bits

/* Initialize the rtc */
void rtc_init(void);

/* Handle the rtc */
void rtc_handler(void);

/* Read call for rtc. Waits until an rtc interrupt is called. */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* Write call for rtc. Sets frequency to given value. */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

/* Open call for rtc. Sets rate to 2Hz. */
int32_t rtc_open(const uint8_t * filename);

/* Close call for rtc. Does nothing right now. */
int32_t rtc_close(int32_t fd);

/* Check if a number is power of two */
int powerOfTwo(int num);

#endif /*_RTC_H*/
