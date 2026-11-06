#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

#define IS_STRING(value)       isObjType(value, OBJ_STRING)

#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj* next;
};

// struct ObjString {
//   Obj obj;
//   int length;
//   char* chars;
// };
// ~~~~~~ challenge 1 start ~~~~~~
// old: char* chars;

// ~~~~~~ challenge 2 start ~~~~~~

struct ObjString {
  Obj obj;
  int length;
  const char* chars;
  bool ownsChars;
  char storedChars[];
};

// ~~~~~~ challenge 2 end ~~~~~~
// ~~~~~~ challenge 1 end ~~~~~~

// ObjString* takeString(char* chars, int length);
// ObjString* copyString(const char* chars, int length);
// void printObject(Value value);

// ~~~~~~ challenge 1 start ~~~~~~
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
ObjString* newString(int length);
// ~~~~~~ challenge 1 end ~~~~~~

// ~~~~~~ challenge 2 start ~~~~~~
ObjString* constantString(const char* chars, int length);
// ~~~~~~ challenge 2 end ~~~~~~

void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif
