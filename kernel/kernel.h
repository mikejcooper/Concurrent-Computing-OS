#ifndef __KERNEL_H
#define __KERNEL_H

#include <stddef.h>
#include <stdint.h>

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#include "interrupt.h"

// Include functionality from newlib, the embedded standard C library.

#include <string.h>

// Include definitions relating to the 2 user programs.

#include "terminal.h"
#include "P0.h"
#include "P1.h"
#include "P2.h"
#include "libc.h"


/* The kernel source code is made simpler by three type definitions:
 *
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
     preservation and restoration prologue and epilogue,
 * - a type that captures a process identifier, which is basically an
 *   integer, and
 * - a type that captures a process PCB.
 */

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef int pid_t;

typedef struct {
  pid_t pid;
  int priority;
  ctx_t ctx;
} pcb_t;

// typedef struct {
//   pid_t pidA;
//   pid_t pidB;
//   char info[100];
//   int flagA;
//   int flagB;
// } ipc_t;


void exitProcessX(int pid);
void printProcesses();
void clearStackSegment(int sp);\
int getFreeStackPosition();
void killCurrentProcess();
pid_t getMemSlot();
void initialisePCBS();
void setChildPCB( pid_t cpid, pid_t ppid, ctx_t* ctx);
void setPCB( uint32_t pc, uint32_t sp, int priority);
void setTimer();
void scheduler( ctx_t* ctx );
void kernel_handler_irq( ctx_t* ctx     );
void kernel_handler_svc( ctx_t* ctx, uint32_t id );
void kernel_handler_rst( ctx_t* ctx              );
void increasePriority(int pid, int priority);
void priorityDistribution(int pid);
int getPriorityLow();
int getPriorityHigh();




#endif
