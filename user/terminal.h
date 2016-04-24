#ifndef __TERMINAL_H
#define __TERMINAL_H

#include <stddef.h>
#include <stdint.h>

#include "libc.h"
#include "kernel.h"

#include "P0.h"
#include "P1.h"
#include "P2.h"
#include "P3.h"




// define symbols for P0 entry point and top of stack
extern void (*entry_terminal)(); 
extern uint32_t tos_terminal;
void P0();
void P1();
void P2();
void P3();


#endif
