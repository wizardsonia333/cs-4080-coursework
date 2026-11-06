#include <stdio.h>
#include "vm.h"
#include "table.h"
#include "object.h"

int main() {
  initVM();

  // create a table for the poodle info
  Table dogs;
  initTable(&dogs);
  Value result;

  // test different key types: number, boolean, nil, and string
  tableSet(&dogs, NUMBER_VAL(5), NUMBER_VAL(100));
  tableSet(&dogs, BOOL_VAL(true), BOOL_VAL(true));
  tableSet(&dogs, NIL_VAL, NUMBER_VAL(0));
  tableSet(&dogs, OBJ_VAL(copyString("Sonia", 5)),
           OBJ_VAL(copyString("Poodle mom", 10)));

  printf("Sonia's Poodle Report Card\n");

  // retrieve values using each key type
  tableGet(&dogs, NUMBER_VAL(5), &result);
  printf("Poodle weight (lbs): 5\n");
  printf("Cuteness level: ");
  printValue(result);
  printf("\n");

  tableGet(&dogs, BOOL_VAL(true), &result);
  printf("Spoiled? ");
  printValue(result);
  printf("\n");

  tableGet(&dogs, NIL_VAL, &result);
  printf("Personal space: ");
  printValue(result);
  printf("\n");

  tableGet(&dogs, OBJ_VAL(copyString("Sonia", 5)), &result);
  printf("Sonia's job: ");
  printValue(result);
  printf("\n");

  // free the table and memory
  freeTable(&dogs);
  freeVM();
  return 0;
}