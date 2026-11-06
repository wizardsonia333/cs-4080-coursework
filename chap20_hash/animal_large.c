
 // this program simulates a larger collection of 50000 animal videos
 // each number represents a video id stored in the hash table
 // it does 200000 searches to compare the speed with the smaller table

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

  // adds 50000 animal videos
  for (int i = 0; i < 50000; i++) {
    tableSet(&videos, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  // times the searches
  clock_t start = clock();

  for (int i = 0; i < 200000; i++) {
    if (tableGet(&videos, NUMBER_VAL(i % 50000), &result)) {
      found++;
    }
  }

  // shows the results
  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
  printf("Large animal video table\n");
  printf("Videos found: %d\n", found);
  printf("Time: %f seconds\n", seconds);

  // frees memory
  freeTable(&videos);
  freeVM();
  return 0;
}