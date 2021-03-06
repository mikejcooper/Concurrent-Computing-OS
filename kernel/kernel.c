#include "kernel.h"
#define process_MAX 10

/* Since we *know* there will be 2 processes, stemming from the 2 user 
 * programs, we can 
 * 
 * - allocate a fixed-size process table (of PCBs), and use a pointer
 *   to keep track of which entry is currently executing, and
 * - employ a fixed-case of round-robin scheduling: no more processes
 *   can be created, and neither is able to complete.
 */

pcb_t pcb[ 10 ];
pcb_t *current = NULL;
uint32_t volatile stack[100] = {0};
int volatile userProcesses[10] = {0};
int volatile sharedVariable = 0;

void kernel_handler_rst( ctx_t* ctx              ) { 
  /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *    
   * - the CPSR value of 0x50 means the processor is switched into USR 
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack. 
   */

  // PL011_puts( UART0, "Kernal Reset \n",14);
  stack[0] = tos_terminal;
  setTimer();  
  initialisePCBS();
  irq_enable();
  setPCB((uint32_t)entry_terminal,tos_terminal,0);

  // Set start point:
  current = &pcb[ 0 ]; 
  memcpy( ctx, &current->ctx, sizeof( ctx_t ) );



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
      scheduler( ctx );
      priorityDistribution(current -> pid);

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
    case 0x02 : { // read
      char*  volatile input = ( char* )( ctx->gpr[ 0 ] );
      int index     = 0;
      int enter    = 0;

      while( enter == 0 ){
        input[ index ] = PL011_getc( UART0 );
        if (input[ index ] == '\r'){
          enter = 1;
        } 
        else if (input[ index ] == '\177'){
          PL011_putc( UART0, '\b' );
          PL011_putc( UART0, ' ' );
          PL011_putc( UART0, '\b' );
          index--;
        }
        else {
          PL011_putc( UART0, input[ index ] );
          index++;
        } 
      }
      ctx -> gpr[ 0 ] = index - 1; 
      break;
    }
    case 0x03 : { // fork()
      pid_t volatile ppid  = current -> pid;
      pid_t volatile cpid = getMemSlot();
      ctx -> gpr[ 0 ] = cpid; 
      setChildPCB(cpid, ppid, ctx);
      scheduler( ctx );
      break;
    }
    case 0x04 : { // kill()
      killCurrentProcess();
      scheduler( ctx );
      break;
    }             
    case 0x05 : { // getProcesses()
      printProcesses();
      break;
    }
    case 0x06 : { // exit
      int pid = ctx->gpr[ 1 ];
      exitProcessX(pid);
      ctx -> gpr[ 0 ] = 1;
      break; 
    } 
    case 0x07 : { // increase priority
      int pid = ctx->gpr[ 1 ];
      int priority = ctx->gpr[ 2 ];
      increasePriority(pid, priority);
      ctx -> gpr[ 0 ] = 1; 
      break;
    }
    case 0x08 : { // shared int
      ctx -> gpr[ 1 ] = sharedVariable; 
      break;
    }
    case 0x09 : { // shared int
      int value = ctx->gpr[ 1 ];
      sharedVariable = value;
      break;
    }
    default   : { 
      break;
    }
  }
  return;
}


// Handle the Interrupt Requests
void kernel_handler_irq( ctx_t* ctx     ){
  PL011_puts( UART0, "\nInterrupt\n",11);

  // Read interrupt Id
  uint32_t id = GICC0 -> IAR;

  // Handle interrupt then reset Timer
  if ( id == GIC_SOURCE_TIMER0 ) {
    TIMER0 -> Timer1IntClr = 0x01;
    scheduler( ctx ); 
  } else if (id == GIC_SOURCE_UART0){
      pcb[0].priority = 2000; 
      scheduler( ctx );
      priorityDistribution(current -> pid);

      pcb[0].priority = 0;
      PL011_getc( UART0 );
      UART0->ICR = 0x10;
  }

  // Signal that we are done
  GICC0 -> EOIR = id;


}

void scheduler( ctx_t* ctx ) {
  int volatile best = -1;
  int volatile highestPriorityPCB = 0;

  if(current -> pid == 0){
    for (int volatile i = 0; i < process_MAX ; i++ ) {  
      if ( pcb[ i ].priority > best){
        highestPriorityPCB = i;
        best = pcb[ i ].priority;
      }
    }
  }
  else {
    highestPriorityPCB = 0;
    pcb[ current -> pid ].priority = getPriorityLow() - 1;
  }
  

  memcpy( &pcb[ current -> pid ].ctx, ctx, sizeof( ctx_t ) );
  memcpy( ctx, &pcb[highestPriorityPCB].ctx, sizeof( ctx_t ) );
  // update current 
  current = &pcb[ highestPriorityPCB ];
}

void setTimer(){
    // TIMER0->Timer1Load     = 0x00001000; // select period = 2^20 ticks ~= 1 sec // change the 1 to 2 => 2sec -- you can make this much faster and much slower
    // TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
    // TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
    // TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
    // TIMER0->Timer1Ctrl    |= 0x00000080; // enable          Timer1Ctrl
    
    UART0-> IMSC           |= 0x00000010;
    UART0-> CR              = 0x00000301;

    GICC0->PMR             = 0x000000F0; // unmask all            interrupts
    GICD0->ISENABLER[ 1 ] |= 0x00001010; // enable timer          interrupt
    GICC0->CTLR            = 0x00000001; // enable GIC interface
    GICD0->CTLR            = 0x00000001; // enable GIC distributor
}

// Create new Process  
void setPCB( uint32_t pc, uint32_t sp, int priority){
  int p = getMemSlot();
  memset( &pcb[ p ], 0, sizeof( pcb_t ) );
  pcb[ p ].pid      = p;
  pcb[ p ].priority = priority;
  pcb[ p ].ctx.cpsr = 0x50;
  pcb[ p ].ctx.pc   = ( uint32_t )( pc );
  pcb[ p ].ctx.sp   = ( uint32_t )( &sp );
}

void setChildPCB( pid_t cpid, pid_t ppid, ctx_t* ctx){
  int priority = getPriorityHigh() + 1;
  memcpy( &pcb[ cpid ], 0, sizeof( pcb_t ) );
  memcpy( &pcb[ cpid ].ctx, ctx, sizeof( ctx_t ) );
  pcb[ cpid ].pid      = cpid;
  pcb[ cpid ].priority = priority;
  pcb[ cpid ].ctx.sp   = ( uint32_t )( getFreeStackPosition());
  pcb[ cpid ].ctx.gpr[0]   = 0;
}


void initialisePCBS(){
  for ( int volatile i = 0; i <= process_MAX; i++ ) {
    memset( &pcb[ i ], 0, sizeof( pcb_t ) );
    pcb[ i ].priority = -1;
    pcb[ i ].pid = i;
  }
}

pid_t getMemSlot() {
  pid_t volatile cpid = -1;
  for(int volatile i = 0; i < process_MAX; i++){
    if ( pcb[ i ].priority == -1 ) {
      return pcb[i].pid;
    }
  }
}

void killCurrentProcess(){
  pcb[ current -> pid ].priority = -1;
  clearStackSegment(pcb[ current -> pid ].ctx.sp);
}

int getFreeStackPosition(){
  for(int volatile i = 1; i < 100; i++){
    if (stack[i] == 0){
      stack[i] = stack[i-1] + 0x00001000;
      return stack[i];
    }
  }
  return 0;
}

void clearStackSegment(int sp){
  for(int volatile i = 1; i < 100; i++){
    if (stack[i-1] < sp < stack [i]){
      stack[i-1] = 0;
    }
  }
}


void printProcesses(){
  for(int volatile i = 0; i < process_MAX; i++){
    int volatile pid = pcb[i].pid;
    int volatile priority = pcb[i].priority;
    if(priority != -1){
      PL011_puts( UART0, "   Processes:\n",14);
      PL011_puts( UART0, "       ID: ",14);
      writeInt(pid);
      PL011_puts( UART0, " , Priority: ",13);
      writeInt(priority);
      PL011_puts( UART0, "\n",1);
    }
  }
}


void exitProcessX(int pid){
  pcb[ pid ].priority = -1;
  clearStackSegment(pcb[ pid ].ctx.sp);
}

void increasePriority(int pid, int priority){
  pcb[pid].priority = priority;
}

void priorityDistribution(int pid){
  for ( int volatile i = 0; i <= process_MAX; i++ ) {
      if (i != pid && pcb[ i ].priority != -1){
        pcb[ i ].priority = pcb[ i ].priority + 1;
      }
  }
}

int getPriorityLow(){
  int volatile low = 1000;
  for ( int volatile i = 1; i < process_MAX; i++ ) {
      if (pcb[ i ].priority < low && pcb[ i ].priority != -1){
        low = pcb[ i ].priority;
      }
  }
  if(low == 1000){
    return 1;
  } else {
    return low;
  }
}

int getPriorityHigh(){
  int volatile high = 0;
  for ( int volatile i = 1; i < process_MAX; i++ ) {
      if (pcb[ i ].priority > high && pcb[ i ].priority != -1){
        high = pcb[ i ].priority;
      }
  }
  return high;
}




