#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

// ~~~~~~ challenge 1 start ~~~~~~
static bool isEmptyKey(Value key) {
  return IS_OBJ(key) && AS_OBJ(key) == NULL;
}
// ~~~~~~ challenge 1 end ~~~~~~

// ~~~~~~ challenge 1 start ~~~~~~
static uint32_t hashKey(Value key) {
  switch (key.type) {
    case VAL_NIL:
      return 0;
    case VAL_BOOL:
      return AS_BOOL(key) ? 1 : 2;
    case VAL_NUMBER: {
      double number = AS_NUMBER(key);
      if (number == 0) return 0;
      uint64_t bits;
      memcpy(&bits, &number, sizeof(double));
      return (uint32_t)(bits ^ (bits >> 32));
    }
    case VAL_OBJ:
      return AS_STRING(key)->hash;
  }
  return 0;
}
// ~~~~~~ challenge 1 end ~~~~~~

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}
void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}
// NOTE: The "Optimization" chapter has a manual copy of this function.
// If you change it here, make sure to update that copy.

// old: 
// static Entry* findEntry(Entry* entries, int capacity,
//                         ObjString* key) {
//   uint32_t index = key->hash % capacity;
//   Entry* tombstone = NULL;

//   for (;;) {
//     Entry* entry = &entries[index];
//     if (entry->key == NULL) {
//       if (IS_NIL(entry->value)) {
//         // Empty entry.
//         return tombstone != NULL ? tombstone : entry;
//       } else {
//         // We found a tombstone.
//         if (tombstone == NULL) tombstone = entry;
//       }
//     } else if (entry->key == key) {
//       // We found the key.
//       return entry;
//     }

//     index = (index + 1) % capacity;
//   }
// }
// ~~~~~~ challenge 1 start ~~~~~~
static Entry* findEntry(Entry* entries, int capacity,
                        Value key) {
  uint32_t index = hashKey(key) % capacity;
  Entry* tombstone = NULL;
  for (;;) {
    Entry* entry = &entries[index];

    if (isEmptyKey(entry->key)) {
      if (IS_NIL(entry->value)) {
        return tombstone != NULL ? tombstone : entry;
      } else {
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (valuesEqual(entry->key, key)) {
      return entry;
    }
    index = (index + 1) % capacity;
  }
}
// ~~~~~~ challenge 1 end ~~~~~~

// old:
// bool tableGet(Table* table, ObjString* key, Value* value) {
//   if (table->count == 0) return false;

//   Entry* entry = findEntry(table->entries, table->capacity, key);
//   if (entry->key == NULL) return false;

//   *value = entry->value;
//   return true;
// }
// ~~~~~~ challenge 1 start ~~~~~~
bool tableGet(Table* table, Value key, Value* value) {
  if (table->count == 0) return false;
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (isEmptyKey(entry->key)) return false;
  *value = entry->value;
  return true;
}
// ~~~~~~ challenge 1 end ~~~~~~

// old:
// static void adjustCapacity(Table* table, int capacity) {
//   Entry* entries = ALLOCATE(Entry, capacity);
//   for (int i = 0; i < capacity; i++) {
//     entries[i].key = NULL;
//     entries[i].value = NIL_VAL;
//   }

//   table->count = 0;
//   for (int i = 0; i < table->capacity; i++) {
//     Entry* entry = &table->entries[i];
//     if (entry->key == NULL) continue;

//     Entry* dest = findEntry(entries, capacity, entry->key);
//     dest->key = entry->key;
//     dest->value = entry->value;
//     table->count++;
//   }

//   FREE_ARRAY(Entry, table->entries, table->capacity);
//   table->entries = entries;
//   table->capacity = capacity;
// }
// ~~~~~~ challenge 1 start ~~~~~~
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = OBJ_VAL(NULL);
    entries[i].value = NIL_VAL;
  }
  table->count = 0;

  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (isEmptyKey(entry->key)) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }
  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}
// ~~~~~~ challenge 1 end ~~~~~~

// old:
// bool tableSet(Table* table, ObjString* key, Value value) {
//   if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
//     int capacity = GROW_CAPACITY(table->capacity);
//     adjustCapacity(table, capacity);
//   }

//   Entry* entry = findEntry(table->entries, table->capacity, key);
//   bool isNewKey = entry->key == NULL;
//   if (isNewKey && IS_NIL(entry->value)) table->count++;

//   entry->key = key;
//   entry->value = value;
//   return isNewKey;
// }
// bool tableDelete(Table* table, ObjString* key) {
//   if (table->count == 0) return false;

//   // Find the entry.
//   Entry* entry = findEntry(table->entries, table->capacity, key);
//   if (entry->key == NULL) return false;

//   // Place a tombstone in the entry.
//   entry->key = NULL;
//   entry->value = BOOL_VAL(true);
//   return true;
// }
// ~~~~~~ challenge 1 start ~~~~~~
bool tableSet(Table* table, Value key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = isEmptyKey(entry->key);

  if (isNewKey && IS_NIL(entry->value)) table->count++;

  entry->key = key;
  entry->value = value;
  return isNewKey;
}
// ~~~~~~ challenge 1 end ~~~~~~

// ~~~~~~ challenge 1 start ~~~~~~
bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (isEmptyKey(entry->key)) return false;

  entry->key = OBJ_VAL(NULL);
  entry->value = BOOL_VAL(true);
  return true;
}
// ~~~~~~ challenge 1 end ~~~~~~

// old:
// void tableAddAll(Table* from, Table* to) {
//   for (int i = 0; i < from->capacity; i++) {
//     Entry* entry = &from->entries[i];
//     if (entry->key != NULL) {
//       tableSet(to, entry->key, entry->value);
//     }
//   }
// }
// ~~~~~~ challenge 1 start ~~~~~~
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (!isEmptyKey(entry->key)) {
      tableSet(to, entry->key, entry->value);
    }
  }
}
// ~~~~~~ challenge 1 end ~~~~~~

// old:
// ObjString* tableFindString(Table* table, const char* chars,
//                            int length, uint32_t hash) {
//   if (table->count == 0) return NULL;

//   uint32_t index = hash % table->capacity;
//   for (;;) {
//     Entry* entry = &table->entries[index];
//     if (entry->key == NULL) {
//       // Stop if we find an empty non-tombstone entry.
//       if (IS_NIL(entry->value)) return NULL;
//     } else if (entry->key->length == length &&
//         entry->key->hash == hash &&
//         memcmp(entry->key->chars, chars, length) == 0) {
//       // We found it.
//       return entry->key;
//     }

//     index = (index + 1) % table->capacity;
//   }
// }
// ~~~~~~ challenge 1 start ~~~~~~
// this time checks that the key is a string b4 accessing its characters
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;

  for (;;) {
    Entry* entry = &table->entries[index];

    if (isEmptyKey(entry->key)) {
      if (IS_NIL(entry->value)) return NULL;
    } else if (IS_STRING(entry->key)) {
      ObjString* string = AS_STRING(entry->key);

      if (string->length == length &&
          string->hash == hash &&
          memcmp(string->chars, chars, length) == 0) {
        return string;
      }
    }

    index = (index + 1) % table->capacity;
  }
}
// ~~~~~~ challenge 1 end ~~~~~~

