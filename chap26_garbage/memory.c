#include <stdlib.h>
#include <stdio.h>

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  vm.bytesAllocated += newSize - oldSize;
  if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
    collectGarbage();
#endif

    if (vm.bytesAllocated > vm.nextGC) {
      collectGarbage();
    }
  }

  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) exit(1);
  return result;
}
void markObject(Obj* object) {
  if (object == NULL) return;
  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // if (object->isMarked) return;

  // checks the mark bit from compact header
  // ~~~~~~ challenge 2 start ~~~~~~
  // old (for challenge 1):
  // if (getObjMarked(object)) return; 

  // checks if the object is marked for this collection
  if (getObjMarked(object) == vm.currentMark) return;
  // ~~~~~~ challenge 2 end ~~~~~~
  // ~~~~~~ challenge 1 end ~~~~~~

#ifdef DEBUG_LOG_GC
  printf("%p mark ", (void*)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif

  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // object->isMarked = true;

  // marks object reachable
  // ~~~~~~ challenge 2 start ~~~~~~
  // old (for challenge 1):
  // setObjMarked(object, true);

  // marks the object using the current mark value
  setObjMarked(object, vm.currentMark);
  // ~~~~~~ challenge 2 end ~~~~~~
  // ~~~~~~ challenge 1 end ~~~~~~

  if (vm.grayCapacity < vm.grayCount + 1) {
    vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
    vm.grayStack = (Obj**)realloc(vm.grayStack,
                                  sizeof(Obj*) * vm.grayCapacity);

    if (vm.grayStack == NULL) exit(1);
  }

  vm.grayStack[vm.grayCount++] = object;
}
void markValue(Value value) {
  if (IS_OBJ(value)) markObject(AS_OBJ(value));
}
static void markArray(ValueArray* array) {
  for (int i = 0; i < array->count; i++) {
    markValue(array->values[i]);
  }
}
static void blackenObject(Obj* object) {
#ifdef DEBUG_LOG_GC
  printf("%p blacken ", (void*)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif

  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // switch (object->type) {

  // gets object type from compact header
  switch (getObjType(object)) {
  // ~~~~~~ challenge 1 end ~~~~~~
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      markObject((Obj*)closure->function);
      for (int i = 0; i < closure->upvalueCount; i++) {
        markObject((Obj*)closure->upvalues[i]);
      }
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      markObject((Obj*)function->name);
      markArray(&function->chunk.constants);
      break;
    }
    case OBJ_UPVALUE:
      markValue(((ObjUpvalue*)object)->closed);
      break;
    case OBJ_NATIVE:
    case OBJ_STRING:
      break;
  }
}
static void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // printf("%p free type %d\n", (void*)object, object->type);
  printf("%p free type %d\n", (void*)object, getObjType(object));
  // ~~~~~~ challenge 1 end ~~~~~~
#endif

  // ~~~~~~ challenge 1 start ~~~~~~
  // old:
  // switch (object->type) {

  // uses stored type to free right object
  switch (getObjType(object)) {
  // ~~~~~~ challenge 1 end ~~~~~~
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      FREE_ARRAY(ObjUpvalue*, closure->upvalues,
                 closure->upvalueCount);
      FREE(ObjClosure, object);
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      freeChunk(&function->chunk);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_NATIVE:
      FREE(ObjNative, object);
      break;
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, object);
      break;
    }
    case OBJ_UPVALUE:
      FREE(ObjUpvalue, object);
      break;
  }
}
static void markRoots() {
  for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
    markValue(*slot);
  }

  for (int i = 0; i < vm.frameCount; i++) {
    markObject((Obj*)vm.frames[i].closure);
  }

  for (ObjUpvalue* upvalue = vm.openUpvalues;
       upvalue != NULL;
       upvalue = upvalue->next) {
    markObject((Obj*)upvalue);
  }

  markTable(&vm.globals);
  markCompilerRoots();
}
static void traceReferences() {
  while (vm.grayCount > 0) {
    Obj* object = vm.grayStack[--vm.grayCount];
    blackenObject(object);
  }
}

// old function:
// static void sweep() {
//   Obj* previous = NULL;
//   Obj* object = vm.objects;
//   while (object != NULL) {
//     if (object->isMarked) {
//       object->isMarked = false;
//       previous = object;
//       object = object->next;
//     } else {
//       Obj* unreached = object;
//       object = object->next;
//       if (previous != NULL) {
//         previous->next = object;
//       } else {
//         vm.objects = object;
//       }

//       freeObject(unreached);
//     }
//   }
// }
// ~~~~~~ challenge 1 start ~~~~~~
// old sweep() used object->isMarked & object->next directly

static void sweep() {
  Obj* previous = NULL;
  Obj* object = vm.objects;
  while (object != NULL) {
    // ~~~~~~ challenge 2 start ~~~~~~
    // old (for challenge 1):
    // if (getObjMarked(object)) {
    //   setObjMarked(object, false);
    // keeps marked objects w/o resetting the mark bit
    if (getObjMarked(object) == vm.currentMark) {
      previous = object;
      object = getObjNext(object);
    // ~~~~~~ challenge 2 end ~~~~~~
    } else {
      // frees objects that weren't reached
      Obj* unreached = object;
      object = getObjNext(object);
      if (previous != NULL) {
        setObjNext(previous, object);
      } else {
        vm.objects = object;
      }
      freeObject(unreached);
    }
  }
}
// ~~~~~~ challenge 1 end ~~~~~~

void collectGarbage() {
#ifdef DEBUG_LOG_GC
  printf("-- gc begin\n");
  size_t before = vm.bytesAllocated;
#endif

  markRoots();
  traceReferences();
  tableRemoveWhite(&vm.strings);
  sweep();
  // ~~~~~~ challenge 2 start ~~~~~~
  // switches mark value 4 the next collection
  // shows mark values switching b/w GC cycles
  printf("GC used mark: %d\n", vm.currentMark);
  vm.currentMark = !vm.currentMark;
  printf("Next GC mark: %d\n", vm.currentMark);
  // ~~~~~~ challenge 2 end ~~~~~~

  vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
  printf("-- gc end\n");
  printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
         before - vm.bytesAllocated, before, vm.bytesAllocated,
         vm.nextGC);
#endif
}
void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    // ~~~~~~ challenge 1 start ~~~~~~
    // old:
    // Obj* next = object->next;
    // reads next object from compact header
    Obj* next = getObjNext(object);
    // ~~~~~~ challenge 1 end ~~~~~~
    freeObject(object);
    object = next;
  }

  free(vm.grayStack);
}
