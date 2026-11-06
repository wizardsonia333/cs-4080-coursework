
// this program simulates a collection of 50000 old animal videos
// it deletes the videos from the hash table one by one
// this helps test how fast the table handles lots of deletions

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "table.h"

int main() {
  initVM();

  // creates the table
  Table videos;
  initTable(&videos);
  int deleted = 0;

  // adds 50000 animal videos
  for (int i = 0; i < 50000; i++) {
    tableSet(&videos, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  // times the deletions
  clock_t start = clock();

  for (int i = 0; i < 50000; i++) {
    if (tableDelete(&videos, NUMBER_VAL(i))) {
      deleted++;
    }
  }

  // shows the results
  double seconds = (double)(clock() - start) / CLOCKS_PER_SEC;
  printf("Deleting animal videos\n");
  printf("Videos deleted: %d\n", deleted);
  printf("Time: %f seconds\n", seconds);

  // frees memory
  freeTable(&videos);
  freeVM();
  return 0;
}