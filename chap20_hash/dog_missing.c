
// this program simulates a collection of 50000 dog videos
// it searches for video ids that aren't stored in the hash table
// this helps test how fast the table handles missing videos

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "table.h"

int main() {
  initVM();

  // creates the table
  Table videos;
  initTable(&videos);
  Value result;
  int found = 0;

  // adds 50000 dog videos
  for (int i = 0; i < 50000; i++) {
    tableSet(&videos, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  // times the missing searches
  clock_t start = clock();

  for (int i = 0; i < 200000; i++) {
    if (tableGet(&videos, NUMBER_VAL(50000 + i), &result)) {
      found++;
    }
  }

  // shows the results
  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
  printf("Missing dog video test\n");
  printf("Videos found: %d\n", found);
  printf("Time: %f seconds\n", seconds);

  // frees memory
  freeTable(&videos);
  freeVM();
  return 0;
}