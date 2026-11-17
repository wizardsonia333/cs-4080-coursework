#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "object.h"

int main() {
  // now checks if malloc & realloc return aligned addresses
  void* pointers[100];
  int aligned = 1;
  for (int i = 0; i < 100; i++) {
    pointers[i] = malloc(sizeof(ObjString));
    if (pointers[i] == NULL) return 1;
    if (((uintptr_t)pointers[i] & 15) != 0) {
      aligned = 0;
    }
  }

  // checks realloc too bc clox uses it
  for (int i = 0; i < 100; i++) {
    void* result = realloc(pointers[i], sizeof(ObjFunction));
    if (result == NULL) return 1;
    pointers[i] = result;
    if (((uintptr_t)pointers[i] & 15) != 0) {
      aligned = 0;
    }
  }

  // frees everything afterward
  for (int i = 0; i < 100; i++) {
    free(pointers[i]);
  }
  printf("All pointers 16-byte aligned: %s\n",
         aligned ? "yes" : "no");
  return 0;
}