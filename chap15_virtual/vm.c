#include <stdio.h>

#include "common.h"
#include "debug.h"
// ~~~~~~ challenge 3 start ~~~~~~
#include "memory.h"
// ~~~~~~ challenge 3 end ~~~~~~
#include "vm.h"

VM vm; // [one]

// old:
// static void resetStack() {
//   vm.stackTop = vm.stack;
// }
//
// void initVM() {
//   resetStack();
// }
//
// void freeVM() {
// }

// ~~~~~~ challenge 3 start ~~~~~~
// starts w/ an empty stack
static void resetStack() {
  vm.stackCount = 0;
  vm.stackTop = vm.stack;
}

void initVM() {
  vm.stack = NULL;
  vm.stackCapacity = 0;
  resetStack();
}

// frees stack memory when done
void freeVM() {
  FREE_ARRAY(Value, vm.stack, vm.stackCapacity);
  vm.stack = NULL;
  vm.stackCount = 0;
  vm.stackCapacity = 0;
  vm.stackTop = NULL;
}
// ~~~~~~ challenge 3 end ~~~~~~

// ~~~~~~ challenge 3 start ~~~~~~
// makes bigger stack when it gets full
static void growStack() {
  int oldCapacity = vm.stackCapacity;
  int newCapacity = GROW_CAPACITY(oldCapacity);

  vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, newCapacity);
  vm.stackCapacity = newCapacity;
  vm.stackTop = vm.stack + vm.stackCount;
}
// ~~~~~~ challenge 3 end ~~~~~~

// old code:
// void push(Value value) {
//   *vm.stackTop = value;
//   vm.stackTop++;
// }
//
// Value pop() {
//   vm.stackTop--;
//   return *vm.stackTop;
// }
// ~~~~~~ challenge 3 start ~~~~~~
// grows stack if there isn't room
void push(Value value) {
  if (vm.stackCount + 1 > vm.stackCapacity) {
    growStack();
  }

  vm.stack[vm.stackCount] = value;
  vm.stackCount++;
  vm.stackTop = vm.stack + vm.stackCount;
}

// removes & returns top value
Value pop() {
  vm.stackCount--;
  vm.stackTop = vm.stack + vm.stackCount;
  return vm.stack[vm.stackCount];
}
// ~~~~~~ challenge 3 end ~~~~~~

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk,
                           (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_ADD:      BINARY_OP(+); break;
      case OP_SUBTRACT: BINARY_OP(-); break;
      case OP_MULTIPLY: BINARY_OP(*); break;
      case OP_DIVIDE:   BINARY_OP(/); break;
      // ~~~~~~ challenge 4 start ~~~~~~
      // original version for performance comparison
      case OP_NEGATE: push(-pop()); break;

      // optimized version:
      // case OP_NEGATE:
      //   vm.stack[vm.stackCount - 1] =
      //       -vm.stack[vm.stackCount - 1];
      //   break;
      // ~~~~~~ challenge 4 end ~~~~~~
      case OP_RETURN: {
        // ~~~~~~ challenge 4 test start ~~~~~~
        // old: printValue(pop());
        // old: printf("\n");
        pop();
        // ~~~~~~ challenge 4 test end ~~~~~~
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}
void hack(bool b) {
  // Hack to avoid unused function error. run() is not used in the
  // scanning chapter.
  run();
  if (b) hack(false);
}
InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}
