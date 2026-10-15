// ~~~~~~ challenge 4 test start ~~~~~~
#include <stdio.h>
#include <time.h>
// ~~~~~~ challenge 4 test end ~~~~~~
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]) {
  initVM();

  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  constant = addConstant(&chunk, 3.4);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_ADD, 123);

  constant = addConstant(&chunk, 5.6);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_DIVIDE, 123);
  writeChunk(&chunk, OP_NEGATE, 123);

  writeChunk(&chunk, OP_RETURN, 123);

  // ~~~~~~ challenge 4 test start ~~~~~~
  // old: disassembleChunk(&chunk, "test chunk");
  // old: interpret(&chunk);

  // times how long it takes to run
  clock_t start = clock();

  for (int i = 0; i < 1000000; i++) {
    interpret(&chunk);
  }

  clock_t end = clock();

  printf("time: %f seconds\n",
        (double)(end - start) / CLOCKS_PER_SEC);
  // ~~~~~~ challenge 4 test end ~~~~~~

  freeVM();
  freeChunk(&chunk);
  return 0;
}
