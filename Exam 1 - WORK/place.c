#include <stdio.h>
#include <stdlib.h>

void print_elements(char **elems, int count) {
  for (int i = 0; i < count; i++) {
    printf("%s\n", elems[i]);
  }
}

// from notes/HuskyCT
int integerCompare(const void *a, const void *b) {
  // array of pointers, so need to cast * to get the address of the target and get the string stored there
  char *s1 = *(char **)a;
  char *s2 = *(char **)b;

  int val1, val2;

  // sscanf from notes, helps parse strings into integrs
  sscanf(s1, "%d", &val1); // like scanf, but takes tge already given inputs instead of waiting for user
  sscanf(s2, "%d", &val2);

  // if reuturning negative, val1 is smaller
  // if returning positive, val2 is bigger
  // was for the first case to get half credit
  return val2 - val1; 
}

int main(int argc, char *argv[]) {
  // stopping segmentation faults
  if (argc < 2) {
    perror("Usage : ./place <strings>"); // added from notes, rushed
    exit(1);
  }

  qsort(&argv[1], argc - 1, sizeof(char *), integerCompare);

  print_elements(&argv[1], argc - 1);

  return 0;
}
