#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"

#define PASS 1
#define FAIL 0

#define DEFAULT_RTC_FREQ	512

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* IDT exception Test
 *
 * Test IDT exception handler through interrupts
 * Inputs: None
 * Outputs: FAIL/error
 * Side Effects: Could cause Division Error Exception
 * Coverage: IDT division error
 * Files: idt.h/c
 */
int idt_exc_test(){
	TEST_HEADER;
	asm volatile("int $0x02");
	return FAIL;
}

/* IDT DE Test
 *
 * Test IDT handler by causing division error exception
 * Inputs: None
 * Outputs: FAIL/error
 * Side Effects: Could cause Division Error Exception
 * Coverage: IDT division error
 * Files: idt.h/c
 */
int idt_de_test(){
	TEST_HEADER;
	int den = 1;
	int nu = 0;
	den = den / nu;
	return FAIL;
}

/* paging_video_test
 *
 * Testing valid video memory address
 * from 0xB8000 to 0xB9000(4KB size)
 * Inputs: None
 * Outputs: PASS/exception
 * Side Effects: Page fault exception may occur.
 * Coverage: paging
 * Files: page.c/h
 */

 int paging_video_test(){

 	int *c;
 	int b;
 	for(c = (int*)PG_VID_START; c < (int*)PG_VID_END; c+=PG_STEP)
 	b = *(c);

 	return PASS;
 }

 /* paging_kernel_test
  *
  * Testing valid kernel memory address
  * from 0x400000 to 0x800000(4MB size)
  * Inputs: None
  * Outputs: PASS/exception
  * Side Effects: Page fault exception may occur.
  * Coverage: paging
  * Files: page.c/h
  */
int paging_kernel_test(){

	int *c;
	int b;
	for(c = (int*)PG_KER_START; c < (int*)PG_KER_END; c+=PG_STEP)
	b = *(c);

	return PASS;
}

/* page_fault_test
 *
 * Testing invalid non-present address at 8MB
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: page-fault exception occurs
 * Coverage: paging
 * Files: page.c/h
 */
int page_fault_test() {
	// int *c =  (int*)PG_KER_END;	// should cause Page Fault exception
	// int *c =  (int*)PG_VID_END;
	int *c =  (int*)0x7FFFFF;
	int b;
	b = *(c);

	return FAIL;

}

/* Checkpoint 2 tests */

/* terminal_write_test
 *
 * Testing terminal_write function
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: output the test buffer to the screen
 */
int terminal_write_test() {

	clear();

	/* test buffer, which has 31 rows and 17 columns */
	uint8_t test_buf[17*31] = {'0', '0', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '1', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '2', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '3', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '4', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '5', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '6', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '7', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '8', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'0', '9', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '0', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '1', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '2', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '3', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '4', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '5', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '6', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '7', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '8', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'1', '9', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '0', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '1', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '2', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '3', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '4', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '5', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '6', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '7', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '8', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'2', '9', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n',
							'3', '0', 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '!', '!', '\n' };

	/* the number of bytes to be written */
	int input = 17*31;

	/* Return PASS if the number of bytes is the same for both input and output */
	if (terminal_write(0,test_buf, input) == input);
		return PASS;

	/* Otherwise return fail */
	return FAIL;
}

/* terminal_write_test
 *
 * Testing terminal_write function
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: output the test buffer to the screen
 */
int terminal_read_test() {
	/* Read from the keyboard buffer continuously */
	while (1){
		/* call terminal_read when there is * new line(s) in the buffer */
		while (terminal_t[terminal_num].buf_flag < 1) {};
		/* call terminal_read */
		while (1) {
			/* 128 is the size of keyboard buffer */
			uint8_t buf[KEYBOARD_BUF_SIZE];
			/* the number of bytes of read/write */
			int32_t input, output;

			/* read from the keyboard buffer */
			input = terminal_read(0, buf, KEYBOARD_BUF_SIZE);

			/* print the output */
			printf("The input  length is: %d\n", input);
			printf("The input command is: ");
			/* output the buffer read from the keyboard buffer */
			output = terminal_write(0, buf, input);
			printf("The output length is: %d\n", output);

			/* Return FAIL if the numbers of bytes differ */
			if (input != output)
				return FAIL;
			/* Break if the keyboard buffer is clear */
			if (terminal_t[terminal_num].buf_flag == 0)
				break;
		}
	}
	return PASS;
}

/* test_dir_file
 * Inputs: none
 * Outputs:none
 * Side Effects: print list of file information
 * Coverage: file system
 * Files: filesystem.c/h
 */
// void test_dir_file() {
// 	dir_read();
//  }

/* test_read_file
 * Inputs: file name
 * Outputs:none
 * Side Effects: Testing file system functionalities
 * by calling read_dentry_by_name and read_data
 * Specifically, contents of executable, txt files should be output to screen
 * Coverage: file system
 * Files: filesystem.c/h
 */
// void test_read_file(const uint8_t* fname) {
// 	clear();
// 	dir_open(fname);
// 	uint32_t inode_start = (uint32_t)to_test + BLOCK_SIZE_IN_BYTES;
// 	inode_t* cur_inode =(inode_t*) (inode_start + local.inode_num * BLOCK_SIZE_IN_BYTES);
// 	int32_t len = cur_inode->length;
// 	char buf [len];
// 	len = file_read(buf, len);
// 	int i;
// 	for (i = 0; i < len; i++) {
// 		putc(buf[i]);
// 	}
// 	printf("\nfile_name: %s\n", fname);
// }

/* rtc_call_test
 * Inputs: none
 * Outputs:none
 * Side Effects: Clears screen. Prints 1 in different frequencies.
 * Coverage: rtc_write, rtc_read, rtc_open
 * Files: rtc.c/h
 */
int rtc_call_test() {
	// rtc_write
	clear();
	int i, frq;
	for (frq = 2; frq <= MAX_RTC_FREQ; frq <<= 1) {
		printf("%dHz:", frq);
		rtc_write(0, &frq, 0);
		for (i = 0; i < frq; i++) {
			putc('1');
			rtc_read(0, 0, 0);
		}
		clear();
	}

	// rtc_open
	printf("open:");
	rtc_open(0);
	for (i = 0; i < 4; i++) {
			putc('1');
			rtc_read(0, 0, 0);
	}
	clear();

	// rtc_write - invalid frequency
	frq = 120;
	rtc_write(0, &frq, 0);
	printf("Invalid:%d (set to 512Hz):", frq);
	for (i = 0; i < DEFAULT_RTC_FREQ; i++) {
			putc('1');
			rtc_read(0, 0, 0);
	}
	clear();
	printf("open:");
	rtc_open(0);
	for (i = 0; i < 4; i++) {
			putc('1');
			rtc_read(0, 0, 0);
	}
	clear();
	frq = 2048;
	rtc_write(0, &frq, 0);
	printf("Invalid:%d (set to 512Hz):", frq);
	for (i = 0; i < DEFAULT_RTC_FREQ; i++) {
			putc('1');
			rtc_read(0, 0, 0);
	}
	clear();

	return PASS;
}
/* Checkpoint 3 tests */
int system_call_test() {
	return PASS;
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* CP1 */
	// TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	// TEST_OUTPUT("idt_exc_test", idt_exc_test());
	// TEST_OUTPUT("idt_de_test", idt_de_test());
	// TEST_OUTPUT("page_fault_test", page_fault_test());
	// TEST_OUTPUT("paging_video_test", paging_video_test());
	// TEST_OUTPUT("paging_kernel_test", paging_kernel_test());
	/* CP2 */
	// TEST_OUTPUT("terminal_write_test", terminal_write_test());
	// test_dir_file();
	// test_read_file((const uint8_t *)"cat");
	// TEST_OUTPUT("rtc_call_test", rtc_call_test());
	// TEST_OUTPUT("terminal_read_test", terminal_read_test());
	/* CP3 */
	//TEST_OUTPUT("system_call_test", system_call_test);
}
