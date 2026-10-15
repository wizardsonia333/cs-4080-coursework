#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  // ~~~~~~ challenge 2 start ~~~~~~
  OP_CONSTANT_LONG,
  // ~~~~~~ challenge 2 end ~~~~~~
  OP_RETURN,
} OpCode;

// ~~~~~~ challenge 1 start ~~~~~~
// stores where each new source line starts in the bytecode
typedef struct {
  int offset;
  int line;
} LineStart;
// ~~~~~~ challenge 1 end ~~~~~~

typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  // ~~~~~~ challenge 1 start ~~~~~~
  // old: int* lines;

  // keeps track of the compressed line records separately
  int lineCount;
  int lineCapacity;
  LineStart* lines;
  // ~~~~~~ challenge 1 end ~~~~~~
  ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
// ~~~~~~ challenge 2 start ~~~~~~
// writes constant using the short/long instruction
void writeConstant(Chunk* chunk, Value value, int line);
// ~~~~~~ challenge 2 end ~~~~~~

// ~~~~~~ challenge 1 start ~~~~~~
// gets source line for given bytecode pos.
int getLine(Chunk* chunk, int instruction);
// ~~~~~~ challenge 1 end ~~~~~~

#endif
