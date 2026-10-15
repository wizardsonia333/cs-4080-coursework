#include <stdlib.h>

// ~~~~~~ challenge 3 start ~~~~~~
#include <string.h>
#include <stddef.h>
// ~~~~~~ challenge 3 end ~~~~~~

#include "memory.h"

// old function:
// void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
//   if (newSize == 0) {
//     free(pointer);
//     return NULL;
//   }
//
//   void* result = realloc(pointer, newSize);
//   if (result == NULL) exit(1);
//   return result;
// }

// ~~~~~~ challenge 3 start ~~~~~~

// 1 big block of memory
#define POOL_SIZE (1024 * 1024)
#define MAX_BLOCKS 1000

// keeps track of each piece of memory
typedef struct {
  size_t start;
  size_t size;
  int free;
} Block;

static unsigned char* pool = NULL;
static Block blocks[MAX_BLOCKS];
static int blockCount = 0;
static size_t used = 0;

// rounds sizes up so memory is properly aligned
static size_t alignSize(size_t size) {
  size_t align = _Alignof(max_align_t);
  size_t extra = size % align;
  if (extra != 0) {
    size += align - extra;
  }
  return size;
}

// gets big memory block one time
void initMemory(void) {
  if (pool != NULL) return;
  pool = malloc(POOL_SIZE);
  if (pool == NULL) exit(1);
  blockCount = 0;
  used = 0;
}

// finds somewhere to put the new data
static void* getMemory(size_t size) {
  if (size > POOL_SIZE) exit(1);
  size = alignSize(size);
  // checks if an old block can be reused
  for (int i = 0; i < blockCount; i++) {
    if (blocks[i].free && blocks[i].size >= size) {
      blocks[i].free = 0;
      return pool + blocks[i].start;
    }
  }
  // otherwise uses space at the end
  if (blockCount >= MAX_BLOCKS) exit(1);
  if (size > POOL_SIZE - used) exit(1);
  blocks[blockCount].start = used;
  blocks[blockCount].size = size;
  blocks[blockCount].free = 0;
  blockCount++;
  used += size;
  return pool + blocks[blockCount - 1].start;
}

// marks an old block as available again
static void giveBack(void* pointer) {
  if (pointer == NULL) return;
  for (int i = 0; i < blockCount; i++) {
    if (pool + blocks[i].start == pointer) {
      blocks[i].free = 1;
      return;
    }
  }
  exit(1);
}

// realloc & free
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    giveBack(pointer);
    return NULL;
  }
  if (pointer == NULL) {
    return getMemory(newSize);
  }
  // resize, get new space & copy
  void* result = getMemory(newSize);
  size_t copySize = oldSize;
  if (newSize < copySize) {
    copySize = newSize;
  }
  memcpy(result, pointer, copySize);
  giveBack(pointer);
  return result;
}
// ~~~~~~ challenge 3 end ~~~~~~
