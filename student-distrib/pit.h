/* Reference: http://www.osdever.net/bkerndev/Docs/pit.htm */

#ifndef _PIT_H
#define _PIT_H

#include "idt.h"

/* Initialize the pit */
void pit_init(void);

/* Handles the pit */
void pit_handler(void);

#endif
