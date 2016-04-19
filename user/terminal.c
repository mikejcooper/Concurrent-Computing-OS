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

void output(){
  if ( compareInput("./P0\r", input) == 1 ){ 
    // if ( pid == 0 ) { // Case in Child process
      int a = fork();
      P0();
      yield();
    // }
  } else if( compareInput("./P1\r", input) == 1 ){ 
    // if ( pid == 0 ) { // Case in Child process
      P1();
      yield();
    // }
  } else if( compareInput("./P1\r", input) == 1 ){ 
      // if ( pid == 0 ) { // Case in Child process
        P1();
        yield();
      // }
  }
  else {
    write( 0, "-bash: ", 7);
    // for(int i = 0; input[i] != '/r'; i++){
    //     write (0, input[i], 1);    
    // }
    write( 0, "command not found\n", 18);
    return;
  }
}

void terminal() {


  while (1){
    write( 0, "terminal$ ", 10);
    read( input );
    write (0, "\n", 1);
    output( input );
  }

  return;
}

void ( *entry_terminal )() = &terminal;

