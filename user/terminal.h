#ifndef __TERMINAL_H
#define __TERMINAL_H

#include <stddef.h>
#include <stdint.h>

#include "libc.h"
#include "kernel.h"


// define symbols for P0 entry point and top of stack
extern void (*entry_terminal)(); 
extern uint32_t tos_terminal;

#endif
