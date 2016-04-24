#ifndef __LIBC_H
#define __LIBC_H

#include <stddef.h>
#include <stdint.h>

// cooperatively yield control of processor, i.e., invoke the scheduler
void yield();

// write n bytes from x to the file descriptor fd
int write( int fd, void* x, size_t n );

void writeInt(int x);

void kill();

int read(void *input);

int fork();

void processes();

int exit(int pid);

int increasePrior(int pid, int priority);

int shared();

int writeShared(int value);



#endif
