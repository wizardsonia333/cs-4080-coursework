#include <stdlib.h>

// ~~~~~~ challenge 3 start ~~~~~~
#include <stdio.h>
// ~~~~~~ challenge 3 end ~~~~~~

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
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

// old reference tracking functions I'm not using
// // ~~~~~~ challenge 3 start ~~~~~~
// // adds a reference to an object
// void retainObject(Obj* object) {
//   if (object == NULL) return;
//   object->refCount++;
// }

// // removes a reference from an object
// void releaseObject(Obj* object) {
//   if (object == NULL) return;
//   if (object->refCount > 0) {
//     object->refCount--;
//   }
// }
// // ~~~~~~ challenge 3 end ~~~~~~

void markObject(Obj* object) {
  if (object == NULL) return;
  if (object->isMarked) return;

#ifdef DEBUG_LOG_GC
  printf("%p mark ", (void*)object);
  printValue(OBJ_VAL(object));
  printf("\n");
#endif

  object->isMarked = true;

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

  switch (object->type) {
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
  printf("%p free type %d\n", (void*)object, object->type);
#endif

  switch (object->type) {
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

// ~~~~~~ challenge 3 start ~~~~~~
// counts references to an object
static void countObject(Obj* object) {
  if (object != NULL) {
    object->refCount++;
  }
}
static void countValue(Value value) {
  if (IS_OBJ(value)) {
    countObject(AS_OBJ(value));
  }
}
// recounts references b4 collecting
static void countReferences() {
  // reset all counts
  for (Obj* object = vm.objects; object != NULL;
       object = object->next) {
    object->refCount = 0;
  }
  // counts stack references
  for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
    countValue(*slot);
  }
  // counts function calls
  for (int i = 0; i < vm.frameCount; i++) {
    countObject((Obj*)vm.frames[i].closure);
  }
  // counts open upvalues
  for (ObjUpvalue* upvalue = vm.openUpvalues;
       upvalue != NULL; upvalue = upvalue->next) {
    countObject((Obj*)upvalue);
  }
  // counts global variables
  for (int i = 0; i < vm.globals.capacity; i++) {
    Entry* entry = &vm.globals.entries[i];
    if (entry->key == NULL) continue;

    countObject((Obj*)entry->key);
    countValue(entry->value);
  }
  // counts references inside objects
  for (Obj* object = vm.objects; object != NULL;
       object = object->next) {
    switch (object->type) {
      case OBJ_CLOSURE: {
        ObjClosure* closure = (ObjClosure*)object;
        countObject((Obj*)closure->function);

        for (int i = 0; i < closure->upvalueCount; i++) {
          countObject((Obj*)closure->upvalues[i]);
        }
        break;
      }
      case OBJ_FUNCTION: {
        ObjFunction* function = (ObjFunction*)object;
        countObject((Obj*)function->name);

        for (int i = 0; i < function->chunk.constants.count; i++) {
          countValue(function->chunk.constants.values[i]);
        }
        break;
      }
      case OBJ_UPVALUE:
        countValue(((ObjUpvalue*)object)->closed);
        break;
      case OBJ_NATIVE:
      case OBJ_STRING:
        break;
    }
  }
}
// frees unreachable objects w/ no references
static void freeZeroReferences() {
  int freed = 0;
  Obj* previous = NULL;
  Obj* object = vm.objects;
  while (object != NULL) {
    if (!object->isMarked && object->refCount == 0) {
      Obj* unused = object;
      object = object->next;
      if (previous == NULL) {
        vm.objects = object;
      } else {
        previous->next = object;
      }
      freeObject(unused);
      freed++;
    } else {
      previous = object;
      object = object->next;
    }
  }
  printf("reference counting freed %d objects\n", freed);
}
// ~~~~~~ challenge 3 end ~~~~~~

static void sweep() {
  Obj* previous = NULL;
  Obj* object = vm.objects;
  while (object != NULL) {
    if (object->isMarked) {
      object->isMarked = false;
      previous = object;
      object = object->next;
    } else {
      Obj* unreached = object;
      object = object->next;
      if (previous != NULL) {
        previous->next = object;
      } else {
        vm.objects = object;
      }

      freeObject(unreached);
    }
  }
}
void collectGarbage() {
#ifdef DEBUG_LOG_GC
  printf("-- gc begin\n");
  size_t before = vm.bytesAllocated;
#endif

  markRoots();
  traceReferences();
  tableRemoveWhite(&vm.strings);
  // ~~~~~~ challenge 3 start ~~~~~~
  // old:
  // sweep();
  // counts/collects objects w/ no references
  countReferences();
  freeZeroReferences();
  // collects any remaining unreachable objects
  sweep();
  // ~~~~~~ challenge 3 end ~~~~~~

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
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }

  free(vm.grayStack);
}
