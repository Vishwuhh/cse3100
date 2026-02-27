#include <stdio.h>
#include <stdlib.h>

typedef struct building {
  int index;
  struct building *neighbor;
} BuildingSet;

void addToSet(BuildingSet **b, int v) {
  BuildingSet *newNode = (BuildingSet *)malloc(sizeof(BuildingSet));
  if (newNode == NULL) return; 
  
  newNode->index = v;
  newNode->neighbor = *b; 
  *b = newNode;           
}

void removeFromSet(BuildingSet **b) {
  if (*b == NULL) return; 
  
  BuildingSet *temp = *b;
  *b = (*b)->neighbor; 
  free(temp);          
}

int isSetEmpty(BuildingSet *b) {
  return b == NULL;
}

int topOfTheSet(BuildingSet *b) {
  if (b == NULL) return -1; 
  return b->index;
}

void printSet(BuildingSet *b) {
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

void freeSet(BuildingSet **b) {
  while (!isSetEmpty(*b)) {
    removeFromSet(b);
  }
}

BuildingSet *checkViews(int *input, int count, int position) {
  BuildingSet *b = NULL;

  if (position == 0) {
    for (int i = count - 1; i >= 0; i--) {
      while (!isSetEmpty(b) && input[i] >= input[topOfTheSet(b)]) {
        removeFromSet(&b);
      }
      addToSet(&b, i); 
    }
  } else {
    for (int i = 0; i < count; i++) {
      while (!isSetEmpty(b) && input[i] >= input[topOfTheSet(b)]) {
        removeFromSet(&b);
      }
      addToSet(&b, i);
    }
  }

  return b; 
}

int main(int argc, char *argv[]) {
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