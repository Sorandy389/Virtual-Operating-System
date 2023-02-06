#ifndef TESTS_H
#define TESTS_H

#include "types.h"

#define PG_KER_START  0x400000
#define PG_KER_END    0x800000
#define PG_VID_START  0xB8000
#define PG_VID_END    0xB9000
#define PG_STEP       4

// test launcher
void launch_tests();

#endif /* TESTS_H */
