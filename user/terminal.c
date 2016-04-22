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

// void execute(void (*f)){
//   int pid = fork();
//   f()
//   yield();
// }


// void output(char* input){
//   if ( compareInput("./P0\r", input) == 1 ){ 
//       int volatile pid = fork();
//       // int a = 0;
      
//       if (pid == 0){
//         write(0,"Child\n",6);
//         exit();
//       } else{
//         write(0,"Parent\n",7);
//       }
//       // P1();
//       // P0();

//   } else if( compareInput("./P1\r", input) == 1 ){ 
//     // if ( pid == 0 ) { // Case in Child process
//       P1();
//       yield();
//     // }
//   } else if( compareInput("./P2\r", input) == 1 ){ 
//       // if ( pid == 0 ) { // Case in Child process
//         P2();
//         yield();
//       // }
//   }
//   else {
//     write( 0, "-bash: ", 7);
//     // for(int i = 0; input[i] != '/r'; i++){
//     //     write (0, input[i], 1);    
//     // }
//     write( 0, "command not found\n", 18);
//     return;
//   }

//   write(0,"here\n",5);



// }

// void launchProcess(char[] processName){


// }

void terminal() {


  while (1){
    write( 0, "terminal$ ", 10);
    read( input );
    write (0, "\n", 1);

  if ( compareInput("./P0\r", input) == 1 ){ 
      int volatile pid = fork();      
      if (pid == 0){
        write(0,"Child\n",6);
        P0();
      } else{
        write(0,"Parent\n",7);
      }
  } else if( compareInput("./P1\r", input) == 1 ){ 
      int volatile pid = fork();      
      if (pid == 0){
        write(0,"Child\n",6);
        P1();
      } else{
        write(0,"Parent\n",7);
      }
  } else if( compareInput("./P2\r", input) == 1 ){ 
      int volatile pid = fork();      
      if (pid == 0){
        write(0,"Child\n",6);
        P2();
      } else{
        write(0,"Parent\n",7);
      }
  } else if( compareInput("yield\r", input) == 1 ){ 
      yield();
  }
  else {
    write( 0, "-bash: ", 7);
    // for(int i = 0; input[i] != '/r'; i++){
    //     write (0, input[i], 1);    
    // }
    write( 0, "command not found\n", 18);
  }





  }

  return;
}

void ( *entry_terminal )() = &terminal;

