#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "value.h"

// ~~~~~~ challenge 1 start ~~~~~~
// old:
// #define OBJ_TYPE(value) (AS_OBJ(value)->type)
// gets type from new compact header
#define OBJ_TYPE(value) getObjType(AS_OBJ(value))
// ~~~~~~ challenge 1 end ~~~~~~

#define IS_CLOSURE(value)      isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)     isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)       isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)       isObjType(value, OBJ_STRING)

#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))
#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
  OBJ_CLOSURE,
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_STRING,
  OBJ_UPVALUE
} ObjType;

// ~~~~~~ challenge 1 start ~~~~~~
// old:
// struct Obj {
//   ObjType type;
//   bool isMarked;
//   struct Obj* next;
// };
// stores the pointer & object info together
struct Obj {
  uintptr_t data;
};
// ~~~~~~ challenge 1 end ~~~~~~

// ~~~~~~ challenge 1 start ~~~~~~
// gets object type from the first 3 bits
static inline ObjType getObjType(Obj* object) {
  return (ObjType)(object->data & 7);
}
// checks if the object is marked using the 4th bit
static inline bool getObjMarked(Obj* object) {
  return (object->data & 8) != 0;
}
// gets the next pointer w/o the xtra bits
static inline Obj* getObjNext(Obj* object) {
  return (Obj*)(object->data & ~(uintptr_t)15);
}
// changes the object type w/o even changing other bits
static inline void setObjType(Obj* object, ObjType type) {
  object->data = (object->data & ~(uintptr_t)7) | (uintptr_t)type;
}
// here it changes the marked bit
static inline void setObjMarked(Obj* object, bool marked) {
  if (marked) {
    object->data |= 8;
  } else {
    object->data &= ~(uintptr_t)8;
  }
}
// changes the next pointer but keeps type and mark bits
static inline void setObjNext(Obj* object, Obj* next) {
  object->data = ((uintptr_t)next) | (object->data & 15);
}
// ~~~~~~ challenge 1 end ~~~~~~

typedef struct {
  Obj obj;
  int arity;
  int upvalueCount;
  Chunk chunk;
  ObjString* name;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct {
  Obj obj;
  NativeFn function;
} ObjNative;

struct ObjString {
  Obj obj;
  int length;
  char* chars;
  uint32_t hash;
};
typedef struct ObjUpvalue {
  Obj obj;
  Value* location;
  Value closed;
  struct ObjUpvalue* next;
} ObjUpvalue;
typedef struct {
  Obj obj;
  ObjFunction* function;
  ObjUpvalue** upvalues;
  int upvalueCount;
} ObjClosure;

ObjClosure* newClosure(ObjFunction* function);
ObjFunction* newFunction();
ObjNative* newNative(NativeFn function);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
ObjUpvalue* newUpvalue(Value* slot);
void printObject(Value value);

// ~~~~~~ challenge 1 start ~~~~~~
// temporarily commented out for the size test (test3)
// static inline bool isObjType(Value value, ObjType type) {
//   return IS_OBJ(value) && AS_OBJ(value)->type == type;
// }
// ~~~~~~ challenge 1 end ~~~~~~

// ~~~~~~ challenge 1 start ~~~~~~
// now put back- but checks the type using new compact header
static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && getObjType(AS_OBJ(value)) == type;
}
// ~~~~~~ challenge 1 end ~~~~~~

#endif
