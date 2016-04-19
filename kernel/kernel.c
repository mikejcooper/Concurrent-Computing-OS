#include "kernel.h"
#define process_MAX 10

// Dictionary 
// ctx: context - state of OS
// gpr: general purpose register 

pcb_t pcb[ process_MAX ];
pcb_t *current = NULL;
int processNumber = 0;

// Kernel reset
void kernel_handler_rst( ctx_t* ctx              ) { 
  /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *    
   * - the CPSR value of 0x50 means the processor is switched into USR 
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack. 
   */

  PL011_puts( UART0, "Kernal Reset \n",14);
  // setTimer();     
  irq_enable();

  // Blank every process. Set priorities to -1:
  for ( int i = 0; i <= process_MAX; i++ ) {
    memset( &pcb[ i ], 0, sizeof( pcb_t ) );
    pcb[ i ].priority = -1;
  }

  setPCB(entry_P0,tos_P0,1);
  setPCB(entry_P1,tos_P1,1);
  setPCB(entry_P2,tos_P2,1);
  setPCB(entry_terminal,tos_terminal,0);

  // memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
  // pcb[ 0 ].pid      = 0;
  // pcb[ 0 ].ctx.cpsr = 0x50;
  // pcb[ 0 ].ctx.pc   = ( uint32_t )( entry_P0 );
  // pcb[ 0 ].ctx.sp   = ( uint32_t )(  &tos_P0 );

  // memset( &pcb[ 1 ], 0, sizeof( pcb_t ) );
  // pcb[ 1 ].pid      = 1;
  // pcb[ 1 ].ctx.cpsr = 0x50;
  // pcb[ 1 ].ctx.pc   = ( uint32_t )( entry_P1 );
  // pcb[ 1 ].ctx.sp   = ( uint32_t )(  &tos_P1 );

  // memset( &pcb[ 2 ], 0, sizeof( pcb_t ) );
  // pcb[ 2 ].pid      = 2;
  // pcb[ 2 ].ctx.cpsr = 0x50;
  // pcb[ 2 ].ctx.pc   = ( uint32_t )( entry_P2 );
  // pcb[ 2 ].ctx.sp   = ( uint32_t )(  &tos_P2 );

  // memset( &pcb[ 3 ], 0, sizeof( pcb_t ) );
  // pcb[ 3 ].pid      = 3;
  // pcb[ 3 ].ctx.cpsr = 0x50;
  // pcb[ 3 ].ctx.pc   = ( uint32_t )( entry_terminal );
  // pcb[ 3 ].ctx.sp   = ( uint32_t )(  &tos_terminal );

  // Set start point:
  current = &pcb[ 3 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );

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
    case 0x02 :{ //read( void *buffer );

      char*  volatile input = ( char* )( ctx->gpr[ 0 ] );
      int index     = 0;
      int enter    = 0;

      while( enter == 0 ){
        input[ index ] = PL011_getc( UART0 );
        if (input[ index ] == '\r'){
          enter = 1;
        } else if (input[ index ] == '\177'){
          // Backspace - remove characters 
          // move cursor using /r /b or something 
          // then write blank characters 
        }   
        PL011_putc( UART0, input[ index ] );
        index++;
      }

      ctx -> gpr[ 0 ] = index - 1; 
      break;
    }    
    case 0x03 :{ // fork() 
      // pid_t result = getpid(ctx);
      pid_t ppid  = current -> pid;
      pid_t cpid  = setChildMem( ppid );

      if ( cpid != -1) {
        int priority = 1;
        setChildPCB( cpid, ppid, ctx, priority);
        //scheduler( ctx );   
      }


      ctx->gpr[0] = cpid;
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

  PL011_puts( UART0, "Kernal Interrupt \n",18);

  // Read interrupt Id
  uint32_t id = GICC0 -> IAR;

  // Handle interrupt then reset Timer
  if ( id == GIC_SOURCE_TIMER0 ) {
    TIMER0 -> Timer1IntClr = 0x01;
    // scheduler( ctx ); 
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
    memcpy( ctx, &pcb[ 3 ].ctx, sizeof( ctx_t ) );
    current = &pcb[ 3 ];
  } else if ( current == &pcb[ 3 ] ) {
    memcpy( &pcb[ 3 ].ctx, ctx, sizeof( ctx_t ) );
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
void setPCB( uint32_t pc, uint32_t sp, int priority){
  int p = processNumber;
  memset( &pcb[ p ], 0, sizeof( pcb_t ) );
  pcb[ p ].pid      = p;
  pcb[ p ].priority = priority;
  pcb[ p ].ctx.cpsr = 0x50;
  pcb[ p ].ctx.pc   = ( uint32_t )( pc );
  pcb[ p ].ctx.sp   = ( uint32_t )( &sp );
  processNumber++;
}

pid_t getpid(ctx_t* ctx){
    if (!current) {
      return -1;
    } else {
      return current->pid;
    }
}

// Create new Process -- copy parent data into child data
void setChildPCB( pid_t cpid, pid_t ppid, ctx_t* ctx, int priority){
  // Blank pcb cpid before writing into it 
    memset (&pcb[ cpid ], 0, sizeof(pcb_t));

    // Copying parent pcb
    pcb[ cpid ].priority = priority; 
    pcb[ cpid ].pid     = cpid;
    pcb[ cpid ].ctx.pc   = pcb[ ppid ].ctx.pc;
    pcb[ cpid ].ctx.cpsr = pcb[ ppid ].ctx.cpsr;
    pcb[ cpid ].ctx.sp   = pcb[ ppid ].ctx.sp + (cpid - ppid)*0x00001000; 

    memcpy( &pcb[ cpid ].ctx, ctx, sizeof(ctx_t));
}


pid_t setChildMem( pid_t ppid ) {
  pid_t cpid = -1;
  for(int i = 0; i < process_MAX; i++){
    if ( pcb[ i ].priority == (-1) ) {
      pcb[ i ].priority   = ppid;
      cpid        = i; 
      break;
    }
    i++;
  }
}



