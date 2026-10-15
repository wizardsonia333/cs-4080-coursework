#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

// ~~~~~~ challenge 3 start ~~~~~~
// old: #define STACK_MAX 256
// stack will grow when needed now
// ~~~~~~ challenge 3 end ~~~~~~

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  // ~~~~~~ challenge 3 start ~~~~~~
  // old: Value stack[STACK_MAX];

  // keeps track of stack memory & how much is used
  Value* stack;
  int stackCount;
  int stackCapacity;
  // ~~~~~~ challenge 3 end ~~~~~~
  Value* stackTop;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);
void push(Value value);
Value pop();

#endif
