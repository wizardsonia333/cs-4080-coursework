#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
  void* pointers[10];

  // checks 10 diff memory addresses
  for (int i = 0; i < 10; i++) {
    pointers[i] = malloc(32);

    printf("pointer: %p\n", pointers[i]);
    printf("last 4 bits: %d\n", (int)((uintptr_t)pointers[i] & 15));
  }

  // frees all the memory after checking
  for (int i = 0; i < 10; i++) {
    free(pointers[i]);
  }

  return 0;
}