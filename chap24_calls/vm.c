#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// ~~~~~~ challenge 4 start ~~~~~~
#include <math.h>
// ~~~~~~ challenge 4 end ~~~~~~

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"


VM vm; // [one]
// ~~~~~~ challenge 3 start ~~~~~~
// old:
// static Value clockNative(int argCount, Value* args) {
//   return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
// }

// clock normally succeeds
static Value clockNative(int argCount, Value* args, bool* success) {
  *success = true;
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}
// ~~~~~~ challenge 3 end ~~~~~~

// ~~~~~~ challenge 3 start ~~~~~~
// temp native function for testing errors for challenge 3
// expects 1 argument & adds 1 treat if the arg is a number
// otherwise it signals failure
static Value dogTreatsNative(int argCount, Value* args, bool* success) {
  if (!IS_NUMBER(args[0])) {
    *success = false;
    return NIL_VAL;
  }
  *success = true;
  return NUMBER_VAL(AS_NUMBER(args[0]) + 1);
}
// ~~~~~~ challenge 3 end ~~~~~~

// ~~~~~~ challenge 4 start ~~~~~~
// squares a number
static Value squareNative(int argCount, Value* args, bool* success) {
  if (!IS_NUMBER(args[0])) {
    *success = false;
    return NIL_VAL;
  }
  *success = true;
  double n = AS_NUMBER(args[0]);
  return NUMBER_VAL(n * n);
}

// finds the square root of a number
static Value sqrtNative(int argCount, Value* args, bool* success) {
  if (!IS_NUMBER(args[0])) {
    *success = false;
    return NIL_VAL;
  }
  double n = AS_NUMBER(args[0]);
  if (n < 0) {
    *success = false;
    return NIL_VAL;
  }
  *success = true;
  return NUMBER_VAL(sqrt(n));
}

// makes a negative number positive
static Value absoluteNative(int argCount, Value* args, bool* success) {
  if (!IS_NUMBER(args[0])) {
    *success = false;
    return NIL_VAL;
  }
  *success = true;
  double n = AS_NUMBER(args[0]);
  if (n < 0) {
    n = -n;
  }
  return NUMBER_VAL(n);
}
// ~~~~~~ challenge 4 end ~~~~~~

static void resetStack() {
  vm.stackTop = vm.stack;
  vm.frameCount = 0;
}
static void runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = vm.frameCount - 1; i >= 0; i--) {
    CallFrame* frame = &vm.frames[i];
    ObjFunction* function = frame->function;
    size_t instruction = frame->ip - function->chunk.code - 1;
    fprintf(stderr, "[line %d] in ", // [minus]
            function->chunk.lines[instruction]);
    if (function->name == NULL) {
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", function->name->chars);
    }
  }

  resetStack();
}
// old: 
// static void defineNative(const char* name, NativeFn function) {
//   push(OBJ_VAL(copyString(name, (int)strlen(name))));
//   push(OBJ_VAL(newNative(function)));
//   tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
//   pop();
//   pop();
// }
// ~~~~~~ challenge 2 start ~~~~~~
// old:
// static void defineNative(const char* name, NativeFn function) {
static void defineNative(const char* name, NativeFn function, int arity) {
  push(OBJ_VAL(copyString(name, (int)strlen(name))));

  // old:
  // push(OBJ_VAL(newNative(function)));
  push(OBJ_VAL(newNative(function, arity)));

  tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
  pop();
  pop();
}
// ~~~~~~ challenge 2 end ~~~~~~

void initVM() {
  resetStack();
  vm.objects = NULL;

  initTable(&vm.globals);
  initTable(&vm.strings);

  // ~~~~~~ challenge 2 start ~~~~~~
  // old:
  // defineNative("clock", clockNative);

  // here clock doesn't need any args
  defineNative("clock", clockNative, 0);
  // ~~~~~~ challenge 2 end ~~~~~~
  // ~~~~~~ challenge 3 start ~~~~~~
  // registers the temp dog treats function i made
  defineNative("dogTreats", dogTreatsNative, 1);
  // ~~~~~~ challenge 3 end ~~~~~~
  // ~~~~~~ challenge 4 start ~~~~~~
  // registers my new native math functions
  defineNative("square", squareNative, 1);
  defineNative("sqrt", sqrtNative, 1);
  defineNative("absolute", absoluteNative, 1);
  // ~~~~~~ challenge 4 end ~~~~~~
}

void freeVM() {
  freeTable(&vm.globals);
  freeTable(&vm.strings);
  freeObjects();
}
void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}
Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}
static Value peek(int distance) {
  return vm.stackTop[-1 - distance];
}
static bool call(ObjFunction* function, int argCount) {
  if (argCount != function->arity) {
    runtimeError("Expected %d arguments but got %d.",
        function->arity, argCount);
    return false;
  }

  if (vm.frameCount == FRAMES_MAX) {
    runtimeError("Stack overflow.");
    return false;
  }

  CallFrame* frame = &vm.frames[vm.frameCount++];
  frame->function = function;
  frame->ip = function->chunk.code;
  frame->slots = vm.stackTop - argCount - 1;
  return true;
}
static bool callValue(Value callee, int argCount) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
      case OBJ_FUNCTION: // [switch]
        return call(AS_FUNCTION(callee), argCount);
      // old:
      // case OBJ_NATIVE: {
      //   NativeFn native = AS_NATIVE(callee);
      //   Value result = native(argCount, vm.stackTop - argCount);
      //   vm.stackTop -= argCount + 1;
      //   push(result);
      //   return true;
      // }
      case OBJ_NATIVE: {
      // ~~~~~~ challenge 2 start ~~~~~~
      // checks the # of args b4 calling
      ObjNative* nativeObj = (ObjNative*)AS_OBJ(callee);

      if (argCount != nativeObj->arity) {
        runtimeError("expected %d args but got %d",
            nativeObj->arity, argCount);
        return false;
      }
      // ~~~~~~ challenge 2 end ~~~~~~

      NativeFn native = AS_NATIVE(callee);
      // ~~~~~~ challenge 3 start ~~~~~~
      // old:
      // Value result = native(argCount, vm.stackTop - argCount);
      // lets the native function say if it failed
      bool success = true;
      Value result = native(argCount, vm.stackTop - argCount, &success);
      if (!success) {
        runtimeError("native function got an invalid argument");
        return false;
      }
      // ~~~~~~ challenge 3 end ~~~~~~
      vm.stackTop -= argCount + 1;
      push(result);
      return true;
    }
      default:
        break; // Non-callable object type.
    }
  }
  runtimeError("Can only call functions and classes.");
  return false;
}
static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}
static void concatenate() {
  ObjString* b = AS_STRING(pop());
  ObjString* a = AS_STRING(pop());

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  push(OBJ_VAL(result));
}
static InterpretResult run() {
  CallFrame* frame = &vm.frames[vm.frameCount - 1];
  // ~~~~~~ challenge 1 start ~~~~~~
  // keeps the instruction pointer local
  register uint8_t* ip = frame->ip;

  // old:
  // #define READ_BYTE() (*frame->ip++)

  // #define READ_SHORT() \
  //     (frame->ip += 2, \
  //     (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

  // reads instructions from local pointer
#define READ_BYTE() (*ip++)

#define READ_SHORT() \
    (ip += 2, \
    (uint16_t)((ip[-2] << 8) | ip[-1]))
  // ~~~~~~ challenge 1 end ~~~~~~

#define READ_CONSTANT() \
    (frame->function->chunk.constants.values[READ_BYTE()])

#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        /* ~~~~~~ challenge 1 start ~~~~~~ */ \
        frame->ip = ip; \
        /* ~~~~~~ challenge 1 end ~~~~~~ */ \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
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
        // ~~~~~~ challenge 1 start ~~~~~~
    // old:
    // disassembleInstruction(&frame->function->chunk,
    //     (int)(frame->ip - frame->function->chunk.code));
    // uses local pointer 4 debugging
    disassembleInstruction(&frame->function->chunk,
        (int)(ip - frame->function->chunk.code));
    // ~~~~~~ challenge 1 end ~~~~~~
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_NIL: push(NIL_VAL); break;
      case OP_TRUE: push(BOOL_VAL(true)); break;
      case OP_FALSE: push(BOOL_VAL(false)); break;
      case OP_POP: pop(); break;
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(frame->slots[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        frame->slots[slot] = peek(0);
        break;
      }
      case OP_GET_GLOBAL: {
        ObjString* name = READ_STRING();
        Value value;
          if (!tableGet(&vm.globals, name, &value)) {
            // ~~~~~~ challenge 1 start ~~~~~~
            // saves current position- for error reporting
            frame->ip = ip;
            // ~~~~~~ challenge 1 end ~~~~~~
            runtimeError("Undefined variable '%s'.", name->chars);
            return INTERPRET_RUNTIME_ERROR;
          }
        push(value);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        ObjString* name = READ_STRING();
        tableSet(&vm.globals, name, peek(0));
        pop();
        break;
      }
      case OP_SET_GLOBAL: {
        ObjString* name = READ_STRING();
        if (tableSet(&vm.globals, name, peek(0))) {
          tableDelete(&vm.globals, name); // [delete]
          // ~~~~~~ challenge 1 start ~~~~~~
          // saves position 4 error reporting
          frame->ip = ip;
          // ~~~~~~ challenge 1 end ~~~~~~
          runtimeError("Undefined variable '%s'.", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
      case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
      case OP_ADD: {
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double b = AS_NUMBER(pop());
          double a = AS_NUMBER(pop());
          push(NUMBER_VAL(a + b));
        } else {
          // ~~~~~~ challenge 1 start ~~~~~~
          // saves position 4 error reporting
          frame->ip = ip;
          // ~~~~~~ challenge 1 end ~~~~~~
          runtimeError(
              "Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
      case OP_NOT:
        push(BOOL_VAL(isFalsey(pop())));
        break;
      case OP_NEGATE:
        if (!IS_NUMBER(peek(0))) {
          // ~~~~~~ challenge 1 start ~~~~~~
          // saves position 4 error reporting
          frame->ip = ip;
          // ~~~~~~ challenge 1 end ~~~~~~
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      case OP_PRINT: {
        printValue(pop());
        printf("\n");
        break;
      }
      case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        // ~~~~~~ challenge 1 start ~~~~~~
        // old:
        // frame->ip += offset;
        ip += offset;
        // ~~~~~~ challenge 1 end ~~~~~~
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        // ~~~~~~ challenge 1 start ~~~~~~
        // old:
        // if (isFalsey(peek(0))) frame->ip += offset;
        if (isFalsey(peek(0))) ip += offset;
        // ~~~~~~ challenge 1 end ~~~~~~
        break;
      }
      case OP_LOOP: {
        uint16_t offset = READ_SHORT();
        // ~~~~~~ challenge 1 start ~~~~~~
        // old:
        // frame->ip -= offset;
        ip -= offset;
        // ~~~~~~ challenge 1 end ~~~~~~
        break;
      }
      case OP_CALL: {
        int argCount = READ_BYTE();
        // ~~~~~~ challenge 1 start ~~~~~~
        // saves old instruction pointer
        frame->ip = ip;
        // ~~~~~~ challenge 1 end ~~~~~~

        if (!callValue(peek(argCount), argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm.frames[vm.frameCount - 1];
        // ~~~~~~ challenge 1 start ~~~~~~
        // loads the instruction pointer 4 the active function
        ip = frame->ip;
        // ~~~~~~ challenge 1 end ~~~~~~
        break;
      }
      case OP_RETURN: {
        Value result = pop();
        vm.frameCount--;
        if (vm.frameCount == 0) {
          pop();
          return INTERPRET_OK;
        }
        vm.stackTop = frame->slots;
        push(result);
        frame = &vm.frames[vm.frameCount - 1];
        // ~~~~~~ challenge 1 start ~~~~~~
        // now goes back to the caller's instruction pointer
        ip = frame->ip;
        // ~~~~~~ challenge 1 end ~~~~~~

        break;
      }
    }
  }

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}
void hack(bool b) {
  // Hack to avoid unused function error. run() is not used in the
  // scanning chapter.
  run();
  if (b) hack(false);
}
InterpretResult interpret(const char* source) {
  ObjFunction* function = compile(source);
  if (function == NULL) return INTERPRET_COMPILE_ERROR;

  push(OBJ_VAL(function));
  call(function, 0);

  return run();
}
