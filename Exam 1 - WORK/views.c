#include <stdio.h>
#include <stdlib.h>

typedef struct building {
  int index;
  struct building *neighbor;
} BuildingSet;

void addToSet(BuildingSet **b, int v) {
  // allocates memory explicitly to BuildingSet* for clarity
  // BuildingSet *newnode creates newnode on stack for the new building address
  // malloc allocated block of memory on heap
  // used sizeof(BuildingSet) to allocate exact number of bytes for the hea
  BuildingSet *newNode = (BuildingSet *)malloc(sizeof(BuildingSet));
  if (newNode == NULL) return; // checks for edge case
  
  newNode->index = v; // initializes the data
  // links the new building to the rest of the stack, and updates the head based on it
  newNode->neighbor = *b
  *b = newNode;           
}

void removeFromSet(BuildingSet **b) {
  // nothing to remove if there is no value that is in the address of b
  if (*b == NULL) return; 
  
  BuildingSet *temp = *b; // address of current head to free later on
  *b = (*b)->neighbor; // shifts the head up to the next building in stack
  free(temp); // the temp holds the memory of the previous head, need to free to avoid leaks on valgirnd          
}

int isSetEmpty(BuildingSet *b) {
  return b == NULL; // boolean TRUE or FALSE
}

int topOfTheSet(BuildingSet *b) {
  // return a signal value if there is no elements that can be returned
  if (b == NULL) return -1; 
  return b->index;
}

void printSet(BuildingSet *b) {
  // edge case assumption, theres nothing to print if stack is empty, but it is redundant
  if (isSetEmpty(b)) {
    printf("Empty\n");
    return;
  }
  while (b != NULL) {
    printf("%d ", b->index);
    b = b->neighbor;
  }
  printf("\n");
}

// helper, use when cleaning the stack
void freeSet(BuildingSet **b) {
  while (!isSetEmpty(*b)) {
    removeFromSet(b);
  }
}
// montonic stack problem, need to find which buildings cast a shadow
BuildingSet *checkViews(int *input, int count, int position) {
  BuildingSet *b = NULL;

  if (position == 0) {
    // loop if if the light is at the far right or end of the series of buildings
    for (int i = count - 1; i >= 0; i--) { 
      // while the stack isn't empty, checks if the current building is taller than the one at top of stack
      while (!isSetEmpty(b) && input[i] >= input[topOfTheSet(b)]) {
        removeFromSet(&b);
      }
      addToSet(&b, i);  // adds the building with the clear line of sight to the light
    }
  } else {
    // similar logic, but starts from the far left or beginning of series of buildings
    for (int i = 0; i < count; i++) {
      // same logic as first loop
      while (!isSetEmpty(b) && input[i] >= input[topOfTheSet(b)]) {
        removeFromSet(&b);
      }
      addToSet(&b, i); // adds the building with the clear line of sight to the light
    }
  }

  return b; 
}

int main(int argc, char *argv[]) {
  // avoids segmentation fault from lack of arguments
  if (argc < 2) { 
    printf("Usage: ./views <position> <height1> <height2> ...\n");
    return 1;
  }

  int position = atoi(argv[1]);
  int count = argc - 2;
  int *input = malloc(count * sizeof(int));

  for (int i = 0; i < count; i++) {
    input[i] = atoi(argv[i + 2]);
  }

  BuildingSet *b = checkViews(input, count, position);
  printSet(b);

  freeSet(&b);
  free(input);

  return 0;
}
