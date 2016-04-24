#include "P3.h"

void P3() {
  while( 1 ) {
    // compute the Hamming weight of each x for 2^8 < x < 2^24
    int a = shared();
    writeShared(a + 10);
    yield();
  }

  return;
}

// void (*entry_P2)() = &P2;

