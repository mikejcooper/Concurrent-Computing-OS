#include "kernel.h"
#include "../user/libc.h"


/* Since we *know* there will be 2 processes, stemming from the 2 user 
 * programs, we can 
 * 
 * - allocate a fixed-size process table (of PCBs), and use a pointer
 *   to keep track of which entry is currently executing, and
 * - employ a fixed-case of round-robin scheduling: no more processes
 *   can be created, and neither is able to complete.
 */


void kernel_handler_rst( ctx_t* ctx              ) { 
  write(0, "Starting\n", 10);

  scheduler_initialise(ctx);

  write(0, "Configuring Timer\n", 19);

  TIMER0->Timer1Load     = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl    |= 0x00000080; // enable          timer

  write(0, "Configuring UART\n", 17);

  UART0->IMSC           |= 0x00000010; // enable UART    (Rx) interrupt
  UART0->CR              = 0x00000301; // enable UART (Tx+Rx)

  write(0, "Configuring Interrupt Controller\n", 34);

  GICC0->PMR             = 0x000000F0; // unmask all          interrupts
  GICD0->ISENABLER[ 1 ] |= 0x00001010; // enable UART (Rx) and Timer 0 interrupt (see 4-65 RealView PB Cortex-A8 Guide)
  GICC0->CTLR            = 0x00000001; // enable GIC interface
  GICD0->CTLR            = 0x00000001; // enable GIC distributor

  write(0, "Enabling interrupts\n", 21);

  irq_enable();

  write(0, "Relinquishing control\n", 23);

  return;
}


void kernel_handler_irq(ctx_t* ctx) {
  // Step 2: read  the interrupt identifier so we know the source.

  write(UART0, "Handling interrupt\n", 20);
  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if ( id == GIC_SOURCE_TIMER0 ) {
    write( 0, "Timer\n", 7);
    scheduler_run(ctx);
    TIMER0->Timer1IntClr = 0x01;
  }

  else if ( id == GIC_SOURCE_UART0 ) {
    uint8_t x = PL011_getc( UART0 );

    write( UART0, "Received: ", 11 );
    intToCharArrayPrint( x );
    write( 0, '\n',2);

    UART0->ICR = 0x10;
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;
}


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
      scheduler_run( ctx );
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
