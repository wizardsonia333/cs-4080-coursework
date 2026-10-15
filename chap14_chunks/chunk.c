#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  // ~~~~~~ challenge 1 start ~~~~~~
  // starts compressed line records as empty
  chunk->lineCount = 0;
  chunk->lineCapacity = 0;
  // ~~~~~~ challenge 1 end ~~~~~~
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}
void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  // ~~~~~~ challenge 1 start ~~~~~~
  // old: FREE_ARRAY(int, chunk->lines, chunk->capacity);
  // frees the compressed line records
  FREE_ARRAY(LineStart, chunk->lines, chunk->lineCapacity);
  // ~~~~~~ challenge 1 end ~~~~~~
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}
// old function:
// void writeChunk(Chunk* chunk, uint8_t byte, int line) {
//   if (chunk->capacity < chunk->count + 1) {
//     int oldCapacity = chunk->capacity;
//     chunk->capacity = GROW_CAPACITY(oldCapacity);
//     chunk->code = GROW_ARRAY(uint8_t, chunk->code,
//         oldCapacity, chunk->capacity);
//     chunk->lines = GROW_ARRAY(int, chunk->lines,
//         oldCapacity, chunk->capacity);
//   }

//   chunk->code[chunk->count] = byte;
//   chunk->lines[chunk->count] = line;
//   chunk->count++;
// }

// ~~~~~~ challenge 1 start ~~~~~~
void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code,
        oldCapacity, chunk->capacity);

    // ~~~~~~ challenge 1 start ~~~~~~
    // old: chunk->lines = GROW_ARRAY(int, chunk->lines,
    //          oldCapacity, chunk->capacity);
    // ~~~~~~ challenge 1 end ~~~~~~
  }

  chunk->code[chunk->count] = byte;

  // ~~~~~~ challenge 1 start ~~~~~~
  // old: chunk->lines[chunk->count] = line;
  // ~~~~~~ challenge 1 end ~~~~~~

  chunk->count++;

  // ~~~~~~ challenge 1 start ~~~~~~
  if (chunk->lineCount > 0 &&
      chunk->lines[chunk->lineCount - 1].line == line) {
    return;
  }

  // grows line records only when needed
  if (chunk->lineCapacity < chunk->lineCount + 1) {
    int oldCapacity = chunk->lineCapacity;
    chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
    chunk->lines = GROW_ARRAY(LineStart, chunk->lines,
        oldCapacity, chunk->lineCapacity);
  }

  // saves where the new source line starts
  LineStart* lineStart = &chunk->lines[chunk->lineCount++];
  lineStart->offset = chunk->count - 1;
  lineStart->line = line;
  // ~~~~~~ challenge 1 end ~~~~~~
}

// ~~~~~~ challenge 1 start ~~~~~~
// finds source line for a bytecode pos.
int getLine(Chunk* chunk, int instruction) {
  int start = 0;
  int end = chunk->lineCount - 1;

  // searches saved line starts
  while (start <= end) {
    int mid = (start + end) / 2;
    LineStart* current = &chunk->lines[mid];

    if (instruction < current->offset) {
      end = mid - 1;
    } else if (mid == chunk->lineCount - 1 ||
               instruction < chunk->lines[mid + 1].offset) {
      return current->line;
    } else {
      start = mid + 1;
    }
  }

  return -1;
}
// ~~~~~~ challenge 1 end ~~~~~~

int addConstant(Chunk* chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

// ~~~~~~ challenge 2 start ~~~~~~
// writes constant using the short/long instruction
void writeConstant(Chunk* chunk, Value value, int line) {
  int index = addConstant(chunk, value);

  if (index < 256) {
    writeChunk(chunk, OP_CONSTANT, line);
    writeChunk(chunk, (uint8_t)index, line);
  } else {
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    writeChunk(chunk, (uint8_t)(index & 0xff), line);
    writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
    writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
  }
}
// ~~~~~~ challenge 2 end ~~~~~~
