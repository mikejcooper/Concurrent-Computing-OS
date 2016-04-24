#include "terminal.h"

char input[ 101 ]; 


int compareInput( char* string, char* usrInput ){
  while (*string != '\r' && *usrInput != '\r') {
      if (*string != *usrInput){ 
        return -1;
      }
      string++, usrInput++;
    }
  return 1;
}

void terminal() {


  while (1){
    write( 0, "terminal$ ", 10);
    read( input );
    write (0, "\n", 1);

    if ( compareInput("./P0\r", input) == 1 ){ 
        int volatile pid = fork();      
        if (pid == 0){
          // write(0,"Child\n",6);
          P0();
        } else{
          // write(0,"Parent\n",7);
        }
    } else if( compareInput("./P1\r", input) == 1 ){ 
        int volatile pid = fork();      
        if (pid == 0){
          // write(0,"Child\n",6);
          P1();
        } else{
          // write(0,"Parent\n",7);
        }
    } else if( compareInput("./P2\r", input) == 1 ){ 
        int volatile pid = fork();      
        if (pid == 0){
          // write(0,"Child\n",6);
          P2();
        } else{
          // write(0,"Parent\n",7);
        }
    } else if( compareInput("./P3\r", input) == 1 ){ 
        int volatile pid = fork();      
        if (pid == 0){
          // write(0,"Child\n",6);
          P3();
        } else{
          // write(0,"Parent\n",7);
        }
    } else if( compareInput("yield\r", input) == 1 ){ 
        yield();
    } else if( compareInput("process\r", input) == 1 ){ 
        processes();
    } else if( compareInput("exit 0", input) == 1 ){ 
            int a = exit(0);
    } else if( compareInput("exit 1", input) == 1 ){ 
            int a = exit(1);
    } else if( compareInput("exit 2", input) == 1 ){ 
            int a = exit(2);
    } else if( compareInput("exit 3", input) == 1 ){ 
            int a = exit(3);
    } else if( compareInput("exit 4", input) == 1 ){ 
            int a = exit(4);
    } else if( compareInput("exit 5", input) == 1 ){ 
            int a = exit(5);
    } else if( compareInput("exit 6", input) == 1 ){ 
            int a = exit(6);
    } else if( compareInput("exit 7", input) == 1 ){ 
            int a = exit(7);
    } else if( compareInput("exit 8", input) == 1 ){ 
            int a = exit(8);
    } else if( compareInput("exit 9", input) == 1 ){ 
            int a = exit(9);
    } else if( compareInput("print shared", input) == 1 ){ 
            int a = shared();
            writeInt(a);
            write(0,"\n",1);
    } else if( compareInput("priority 1 10", input) == 1 ){ 
            int a = increasePrior(1,10);
    } else if( compareInput("priority 1 1", input) == 1 ){ 
            int a = increasePrior(1,1);
    } else {
      write( 0, "-bash: ", 7);
      write( 0, "command not found\n", 18);
    }
  }

  return;
}

void ( *entry_terminal )() = &terminal;

