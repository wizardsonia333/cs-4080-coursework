#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

// static ObjString* allocateString(char* chars, int length) {
//   ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
//   string->length = length;
//   string->chars = chars;
//   return string;
// }
// ObjString* takeString(char* chars, int length) {
//   return allocateString(chars, length);
// }
// ObjString* copyString(const char* chars, int length) {
//   char* heapChars = ALLOCATE(char, length + 1);
//   memcpy(heapChars, chars, length);
//   heapChars[length] = '\0';
//   return allocateString(heapChars, length);
// }

// ~~~~~~ challenge 1 start ~~~~~~
// old: allocateString() allocated ObjString separately
// old: takeString() passed an existing char array
// old: copyString() allocated another char array

// old:
// static ObjString* allocateString(const char* chars, int length) {
//   ObjString* string = (ObjString*)allocateObject(
//       sizeof(ObjString) + length + 1, OBJ_STRING);
//
//   string->length = length;
//   memcpy(string->chars, chars, length);
//   string->chars[length] = '\0';
//
//   return string;
// }

// ~~~~~~ challenge 2 start ~~~~~~

// old:
// static ObjString* allocateString(int length) {
//   ObjString* string = (ObjString*)allocateObject(
//       sizeof(ObjString) + length + 1, OBJ_STRING);
//
//   string->length = length;
//   string->chars[length] = '\0';
//
//   return string;
// }

static ObjString* allocateString(int length) {
  ObjString* string = (ObjString*)allocateObject(
      sizeof(ObjString) + length + 1, OBJ_STRING);

  string->length = length;
  string->ownsChars = true;
  string->chars = string->storedChars;
  string->storedChars[length] = '\0';

  return string;
}

// ~~~~~~ challenge 2 end ~~~~~~

// old:
// ObjString* takeString(char* chars, int length) {
//   ObjString* string = allocateString(chars, length);
//   FREE_ARRAY(char, chars, length + 1);
//   return string;
// }

// ~~~~~~ challenge 2 start ~~~~~~
// old:
// ObjString* takeString(char* chars, int length) {
//   ObjString* string = allocateString(length);
//   memcpy(string->chars, chars, length);
//   FREE_ARRAY(char, chars, length + 1);
//   return string;
// }

ObjString* takeString(char* chars, int length) {
  ObjString* string = allocateString(length);
  memcpy(string->storedChars, chars, length);
  FREE_ARRAY(char, chars, length + 1);
  return string;
}

// old:
// ObjString* copyString(const char* chars, int length) {
//   ObjString* string = allocateString(length);
//   memcpy(string->chars, chars, length);
//   return string;
// }

ObjString* copyString(const char* chars, int length) {
  ObjString* string = allocateString(length);
  memcpy(string->storedChars, chars, length);
  return string;
}
// ~~~~~~ challenge 2 end ~~~~~~

ObjString* newString(int length) {
  return allocateString(length);
}
// ~~~~~~ challenge 2 start ~~~~~~
ObjString* constantString(const char* chars, int length) {
  ObjString* string = (ObjString*)allocateObject(
      sizeof(ObjString), OBJ_STRING);
  string->length = length;
  string->chars = chars;
  string->ownsChars = false;
  return string;
}
// ~~~~~~ challenge 2 end ~~~~~~

// ~~~~~~ challenge 1 end ~~~~~~

// old:
// void printObject(Value value) {
//   switch (OBJ_TYPE(value)) {
//     case OBJ_STRING:
//       printf("%s", AS_CSTRING(value));
//       break;
//   }
// }

// ~~~~~~ challenge 2 start ~~~~~~
void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING: {
      ObjString* string = AS_STRING(value);
      printf("%.*s", string->length, string->chars);
      break;
    }
  }
}
// ~~~~~~ challenge 2 end ~~~~~~