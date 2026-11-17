#include <stdio.h>
#include "object.h"

int main() {
  // checks total size of the Obj header
  printf("Obj size: %d bytes\n", (int)sizeof(Obj));

  // checks size of each field
  printf("type: %d bytes\n", (int)sizeof(ObjType));
  printf("isMarked: %d bytes\n", (int)sizeof(bool));
  printf("next: %d bytes\n", (int)sizeof(Obj*));

  return 0;
}