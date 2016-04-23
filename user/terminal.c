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
  } else if( compareInput("./P4\r", input) == 1 ){ 
      int volatile pid = fork();      
      if (pid == 0){
        // write(0,"Child\n",6);
        // P4();
      } else{
        // write(0,"Parent\n",7);
      }
  } else if( compareInput("./P5\r", input) == 1 ){ 
      int volatile pid = fork();      
      if (pid == 0){
        // write(0,"Child\n",6);
        // P5();
      } else{
        // write(0,"Parent\n",7);
      }
  } else if( compareInput("yield\r", input) == 1 ){ 
      yield();
  } else if( compareInput("process\r", input) == 1 ){ 
      processes();
  } else if( compareInput("kill 0", input) == 1 ){ 
          int a = exitProcess(0);
  } else if( compareInput("kill 1", input) == 1 ){ 
          int a = exitProcess(1);
  } else if( compareInput("kill 2", input) == 1 ){ 
          int a = exitProcess(2);
  } else if( compareInput("kill 3", input) == 1 ){ 
          int a = exitProcess(3);
  } else if( compareInput("kill 4", input) == 1 ){ 
          int a = exitProcess(4);
  } else if( compareInput("kill 5", input) == 1 ){ 
          int a = exitProcess(5);
  } else if( compareInput("kill 6", input) == 1 ){ 
          int a = exitProcess(6);
  } else if( compareInput("kill 7", input) == 1 ){ 
          int a = exitProcess(7);
  } else if( compareInput("kill 8", input) == 1 ){ 
          int a = exitProcess(8);
  } else if( compareInput("kill 9", input) == 1 ){ 
          int a = exitProcess(9);
  }

  else {
    write( 0, "-bash: ", 7);
    write( 0, "command not found\n", 18);
  }





  }

  return;
}

void ( *entry_terminal )() = &terminal;

