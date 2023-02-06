/* rtc.c - Functions to interact with the rtc
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"

static int rtc_flag[3];

/* rtc_init
 * Description: Initialize the rtc.
 * Input: none
 * Output: none
 */
void rtc_init(void) {

	/* Disable the interrupts */
	cli();

	/* Select the register B and disable the NMI */
	outb(RTCREG_B_DISABLE_NMI, RTC_PORT);

	/* Read the previous data from register B */
	int8_t prev = inb(CMOS_PORT);

	/* Reselect the register B and disable the NMI */
	outb(RTCREG_B_DISABLE_NMI, RTC_PORT);
	/* Set 1 to the Bit 6 of register B to turn on rtc */
	outb(prev | 0x40, CMOS_PORT);

	/* Enable the interrupts */
	sti();

	/* Enable the IRQ of rtc */
	add_irq(IRQ_RTC, rtc_handler);
}

/* rtc_handler
 * Description: handles rtc interrupt calls
 * Input: none
 * Output: none
 */
void rtc_handler(void) {
	/* Disable the interrupts */
	cli();

	/* Send EOI to the IRQ of rtc */
	send_eoi(IRQ_RTC);

	/* Select the register C */
	outb(RTCREG_C, RTC_PORT);
	/* Clean the register C otherwise no more interrupts will happen */
	inb(CMOS_PORT);

	/* test for rtc */
	// test_interrupts();
	int i;
	for (i = 0; i < 3; i++) {
		rtc_flag[i] = 0;
	}
	/* Enable the IRQ of rtc */
	sti();
}

/* rtc_read
 * Description: Read call for rtc. Waits until an rtc interrupt is called.
 * Input: None
 * Output: None
 * Side Effect: Waiting
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {
  rtc_flag[terminal_num] = 1;
  while (rtc_flag[terminal_num]);
  return 0;
}

/* rtc_write
 * Description: Write call for rtc. Sets frequency to given value.
 * Input: freq - frequency to write
 * Output: None
 * Side Effect: Changes frequency of rtc
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
	int freq = *((int*)buf);
  if (!powerOfTwo(freq) || freq > 1024) freq = 512;
  freq >>= 2;
  int8_t rate = 0;
  /* Transcribe frequency to rate */
  while (freq != 0 && rate < MAX_RTC_RATE) {
    freq >>= 1;
    rate++;
  }

  /* Set rate by changing RS values in register A */
  disable_irq(IRQ_RTC);
  outb(RTCREG_A_DISABLE_NMI, RTC_PORT);
  int8_t prev = inb(CMOS_PORT);
  outb(RTCREG_A_DISABLE_NMI, RTC_PORT);
  outb((prev & MASK_L) | ((~rate) & MASK_R), CMOS_PORT);
  enable_irq(IRQ_RTC);
  return 0;
}

/* rtc_close
 * Description: Open call for rtc. Sets rate to 2Hz.
 * Input: none
 * Output: none
 */
int32_t rtc_open(const uint8_t* filename) {
	int freq = 2;
  rtc_write(0, &freq, 0);
  return 0;
}

/* rtc_close
 * Description: Close call for rtc. Does nothing right now.
 * Input: none
 * Output: none
 */
int32_t rtc_close(int32_t fd) {
  return 0;
}

/* powerOfTwo
 * Description: Check if input is power of 2
 * Input: num - number to check
 * Output: 1 - is power of 2, 0 - not
 */
int powerOfTwo(int num) {
	int n = num;
	while (!(n & 0x1)) n >>= 1;
	n >>= 1;
	if (n == 0) return 1;
	return 0;
}
