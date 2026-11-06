#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

// typedef struct {
//   ObjString* key;
//   Value value;
// } Entry;

// ~~~~~~ challenge 1 start ~~~~~~
typedef struct {
  Value key;
  Value value;
} Entry;
// ~~~~~~ challenge 1 end ~~~~~~

typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);

// ~~~~~~ challenge 1 start ~~~~~~

// old: bool tableGet(Table* table, ObjString* key, Value* value);
// old: bool tableSet(Table* table, ObjString* key, Value value);
// old: bool tableDelete(Table* table, ObjString* key);

bool tableGet(Table* table, Value key, Value* value);
bool tableSet(Table* table, Value key, Value value);
bool tableDelete(Table* table, Value key);

// ~~~~~~ challenge 1 end ~~~~~~

void tableAddAll(Table* from, Table* to);
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash);

#endif
