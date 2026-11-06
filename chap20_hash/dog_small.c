
 // this program simulates a small collection of 20 dog videos
 // each # represents a video id stored as a key in the hash table
 // it searches for videos 200000 times to see how fast a small table works

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "table.h"

int main() {
  initVM();

  // create the table
  Table videos;
  initTable(&videos);
  Value result;
  int found = 0;

  // add 20 dog videos
  for (int i = 0; i < 20; i++) {
    tableSet(&videos, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  // time the searches
  clock_t start = clock();

  for (int i = 0; i < 200000; i++) {
    if (tableGet(&videos, NUMBER_VAL(i % 20), &result)) {
      found++;
    }
  }

  // show the results
  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
  printf("Small dog video table\n");
  printf("Videos found: %d\n", found);
  printf("Time: %f seconds\n", seconds);

  // free memory
  freeTable(&videos);
  freeVM();
  return 0;
}