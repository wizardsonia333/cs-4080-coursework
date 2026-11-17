#include <stdio.h>
#include "object.h"

int main() {
  // checks the size of the new compact Obj header
  printf("New Obj size: %d bytes\n", (int)sizeof(Obj));

  return 0;
}