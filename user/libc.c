#include "libc.h"

void yield() {
  asm volatile( "svc #0     \n"  );
}

// pid_t fork() {
//         pid_t r;
//         asm volatile(
//                 "svc #3 \n"
//                 "mov %0, r0 \n"
//                 : "=r" (r)
//                 :
//                 : "r0"
//         );
//         return r;
// }


int fork(){
  int r;
  asm volatile( "svc #3     \n"
                "mov %0, r0 \n"
                : "=r" (r));
  return r;
}

int write( int fd, void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #1     \n"
                "mov %0, r0 \n" 
              : "=r" (r) 
              : "r" (fd), "r" (x), "r" (n) 
              : "r0", "r1", "r2" );

  return r;
}

int read(void *input){
  int r;
  asm volatile( "mov r0, %1 \n"
                "svc #2     \n"
                "mov %0, r0 \n"
                : "=r" (r)
                : "r" (input)
                : "r0");
  return r;
}

void writeInt(int a){

	int value = a;
  char number[100];
  int i = 0;
  while(value > 0) {
    int digit = value % 10;
    number[i] = digit;
    value /= 10;
    i++;
  }
  i--;
  while(i != -1){
    char d = '0' + number[i];
    char* dq = &d;
    write(0,dq,1);
    i--;
  }
  if(a == 0){
    write(0,"0",1);
  }

}