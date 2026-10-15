#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}
static int constantInstruction(const char* name, Chunk* chunk,
                               int offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

// ~~~~~~ challenge 2 start ~~~~~~
// reads 3 bytes used for long constant
static int constantLongInstruction(const char* name, Chunk* chunk,
                                   int offset) {
  int constant = chunk->code[offset + 1] |
                 (chunk->code[offset + 2] << 8) |
                 (chunk->code[offset + 3] << 16);
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 4;
}
// ~~~~~~ challenge 2 end ~~~~~~

static int simpleInstruction(const char* name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
int disassembleInstruction(Chunk* chunk, int offset) {
  printf("%04d ", offset);
  // ~~~~~~ challenge 1 start ~~~~~~
  // old code:
  // if (offset > 0 &&
  //     chunk->lines[offset] == chunk->lines[offset - 1]) {
  //   printf("   | ");
  // } else {
  //   printf("%4d ", chunk->lines[offset]);
  // }

  // gets line using compressed line records
  int line = getLine(chunk, offset);

  if (offset > 0 && line == getLine(chunk, offset - 1)) {
    printf("   | ");
  } else {
    printf("%4d ", line);
  }
  // ~~~~~~ challenge 1 end ~~~~~~

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
     // ~~~~~~ challenge 2 start ~~~~~~
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    // ~~~~~~ challenge 2 end ~~~~~~
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
