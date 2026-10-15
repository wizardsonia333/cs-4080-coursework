#include "common.h"
#include "chunk.h"
#include "debug.h"
// ~~~~~~ challenge 3 start ~~~~~~
#include "memory.h"
// ~~~~~~ challenge 3 end ~~~~~~

int main(int argc, const char* argv[]) {
  Chunk chunk;
  // ~~~~~~ challenge 3 start ~~~~~~
  // sets up memory b4 using it
  initMemory();
  // ~~~~~~ challenge 3 end ~~~~~~
  initChunk(&chunk);

  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_RETURN, 123);

  // ~~~~~~ challenge 2 test start ~~~~~~
  // adds enough constants to test long instr.
  for (int i = 0; i < 256; i++) {
    addConstant(&chunk, i);
  }
  writeConstant(&chunk, 999, 123);
  // ~~~~~~ challenge 2 test end ~~~~~~

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}
