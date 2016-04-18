#include "kernel.h"

/* Since we *know* there will be 2 processes, stemming from the 2 user 
 * programs, we can 
 * 
 * - allocate a fixed-size process table (of PCBs), and use a pointer
 *   to keep track of which entry is currently executing, and
 * - employ a fixed-case of round-robin scheduling: no more processes
 *   can be created, and neither is able to complete.
 */

pcb_t pcb[ 3 ], *current = NULL;
int processNumber = -1;

// Kernel reset
void kernel_handler_rst( ctx_t* ctx              ) { 
  /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *    
   * - the CPSR value of 0x50 means the processor is switched into USR 
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack. 
   */

  PL011_putc( UART0, 'A');
  setTimer();     
  irq_enable();

  // // Blank every process. Set priorities to -1:
  // for ( int i = 0; i < 5; i++ ) {
  //   memset( &pcb[ i ], 0, sizeof( pcb_t ) );
  //   pcb[ i ].priority = -1;
  // }

  // setPCB(entry_P0,tos_P0);
  // setPCB(entry_P1,tos_P1);
  // setPCB(entry_P2,tos_P2);

  memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
  pcb[ 0 ].pid      = 0;
  pcb[ 0 ].ctx.cpsr = 0x50;
  pcb[ 0 ].ctx.pc   = ( uint32_t )( entry_P0 );
  pcb[ 0 ].ctx.sp   = ( uint32_t )(  &tos_P0 );

  memset( &pcb[ 1 ], 0, sizeof( pcb_t ) );
  pcb[ 1 ].pid      = 1;
  pcb[ 1 ].ctx.cpsr = 0x50;
  pcb[ 1 ].ctx.pc   = ( uint32_t )( entry_P1 );
  pcb[ 1 ].ctx.sp   = ( uint32_t )(  &tos_P1 );

  memset( &pcb[ 2 ], 0, sizeof( pcb_t ) );
  pcb[ 2 ].pid      = 1;
  pcb[ 2 ].ctx.cpsr = 0x50;
  pcb[ 2 ].ctx.pc   = ( uint32_t )( entry_P2 );
  pcb[ 2 ].ctx.sp   = ( uint32_t )(  &tos_P2 );


  // Set start point:
  current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );

  PL011_putc( UART0, 'B');


  return;
}

// Handle System Calls 
void kernel_handler_svc( ctx_t* ctx, uint32_t id ) { 
  /* Based on the identified encoded as an immediate operand in the
   * instruction, 
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */



  switch( id ) {
    case 0x00 : { // yield()
      // scheduler( ctx );
      break;
    }
    case 0x01 : { // write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );  
      char*  x = ( char* )( ctx->gpr[ 1 ] );  
      int    n = ( int   )( ctx->gpr[ 2 ] ); 

      for( int i = 0; i < n; i++ ) {
        PL011_putc( UART0, *x++ );
      }
      
      ctx->gpr[ 0 ] = n;
      break;
    }
    default   : { // unknown
      break;
    }
  }
  
  return;
}


// Handle the Interrupt Requests
void kernel_handler_irq( ctx_t* ctx     ){

  PL011_putc( UART0, 'I'); 

  // Read interrupt Id
  uint32_t id = GICC0 -> IAR;

  // Handle interrupt then reset Timer
  if ( id == GIC_SOURCE_TIMER0 ) {
    TIMER0 -> Timer1IntClr = 0x01;
    scheduler( ctx ); 
  }

  // Signal that we are done
  GICC0 -> EOIR = id;
}

void scheduler( ctx_t* ctx ) {
  if      ( current == &pcb[ 0 ] ) {
    memcpy( &pcb[ 0 ].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ 1 ].ctx, sizeof( ctx_t ) );
    current = &pcb[ 1 ];
  }
  else if ( current == &pcb[ 1 ] ) {
    memcpy( &pcb[ 1 ].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ 2 ].ctx, sizeof( ctx_t ) );
    current = &pcb[ 2 ];
  } else if ( current == &pcb[ 2 ] ) {
    memcpy( &pcb[ 2 ].ctx, ctx, sizeof( ctx_t ) );
    memcpy( ctx, &pcb[ 0 ].ctx, sizeof( ctx_t ) );
    current = &pcb[ 0 ];
  }
}

void setTimer(){
    TIMER0->Timer1Load     = 0x00001000; // select period = 2^20 ticks ~= 1 sec // change the 1 to 2 => 2sec -- you can make this much faster and much slower
    TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
    TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
    TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
    TIMER0->Timer1Ctrl    |= 0x00000080; // enable          Timer1Ctrl

    GICC0->PMR             = 0x000000F0; // unmask all            interrupts
    GICD0->ISENABLER[ 1 ] |= 0x00000010; // enable timer          interrupt
    GICC0->CTLR            = 0x00000001; // enable GIC interface
    GICD0->CTLR            = 0x00000001; // enable GIC distributor
}

// Create new Process  
void setPCB( uint32_t pc, uint32_t sp){
  int p = processNumber++;

  memset( &pcb[ p ], 0, sizeof( pcb_t ) );
  pcb[ p ].pid      = p;
  pcb[ p ].ctx.cpsr = 0x50;
  pcb[ p ].ctx.pc   = ( uint32_t )( pc );
  pcb[ p ].ctx.sp   = ( uint32_t )( sp );
}





