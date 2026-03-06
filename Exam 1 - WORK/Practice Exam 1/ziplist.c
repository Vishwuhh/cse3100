//Do not modify starter code
//You may add helper functions if needed
#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef struct Node {
  int data;
  struct Node *next;
} Node;

// Create a new node
Node *createNode(int data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

// Insert at end
void insertEnd(Node **head, int data) {
  Node *newNode = createNode(data);
  if (!*head) {
    *head = newNode;
    return;
  }
  Node *temp = *head;
  while (temp->next)
    temp = temp->next;
  temp->next = newNode;
}

// Print list
void printList(Node *head) {
  while (head) {
    printf("%d ", head->data);
    head = head->next;
  }
  printf("\n");
}


// HELPER FUNCTION: Reverse a Linked List
Node* reverseList(Node* head) {
    // We need 3 pointers because reversing a directed arrow requires us to hold onto the past (prev), 
    // the present (current), and the future (next) simultaneously.
    Node* prev = NULL;
    Node* current = head;
    Node* next = NULL;

    while (current != NULL) {
        // 1. SECURE THE FUTURE: Save the address of the next node before we overwrite the pointer.
        next = current->next;
        // 2. REVERSE THE ARROW: Point the current node backward to the previous node.
        current->next = prev;
        // 3. STEP FORWARD: Shift our "past" and "present" trackers one step to the right.
        prev = current;
        current = next;
    }
    // When current hits NULL, prev is standing on the new head of the reversed list.
    return prev;
}

void zipList(Node **headRef) {

  // 1. SAFETY GUARD: 
  // If the list is completely empty (*headRef == NULL) OR has only 1 node ((*headRef)->next == NULL),
  // there is nothing to zip. Attempting to zip it will cause a SegFault. Exit immediately.
  if (headRef == NULL || *headRef == NULL || (*headRef)->next == NULL) {
      return; 
  }

  // 2. FIND THE MIDDLE (The Tortoise and Hare Algorithm):
  // We launch two pointers from the starting line (the head).
  Node *slow = *headRef;
  Node *fast = *headRef;
  
  // The 'fast' pointer jumps 2 nodes at a time. The 'slow' pointer jumps 1 node at a time.
  // When 'fast' reaches the finish line (NULL), 'slow' will be standing exactly at the halfway mark!
  while (fast->next != NULL && fast->next->next != NULL) {
      slow = slow->next;          // Moves 1 step
      fast = fast->next->next;    // Moves 2 steps
  }

  // 3. THE SPLIT:
  // 'slow' is currently the tail end of the first half of the list.
  // The node directly after 'slow' is the head of the second half. We save its address.
  Node *secondHalf = slow->next;
  
  // We sever the list in two! By setting slow->next to NULL, the first half of the list 
  // now officially terminates here. It is no longer connected to the second half.
  slow->next = NULL; 
  
  // We reverse the second half so we can easily pull nodes from what used to be the end of the list.
  secondHalf = reverseList(secondHalf);

  // 4. THE MERGE (The "Zipper"):
  Node *firstHalf = *headRef;
  
  // We keep weaving nodes together until we run out of nodes in the second half.
  while (secondHalf != NULL) {
      // STEP A: SECURE THE FUTURES
      // We are about to change the ->next pointers for both lists. 
      // We MUST save the remainder of the chains, or they will be lost to the void.
      Node *temp1 = firstHalf->next;
      Node *temp2 = secondHalf->next;

      // STEP B: THE WEAVE
      // The current node from the first half reaches across and grabs the current node from the second half.
      firstHalf->next = secondHalf;
      
      // The current node from the second half reaches across and grabs the NEXT node in the first half 
      // (which we safely stored in temp1).
      secondHalf->next = temp1;

      // STEP C: ADVANCE DOWN THE CHAINS
      // We move our main trackers forward to the original "next" nodes we saved in Step A.
      firstHalf = temp1;
      secondHalf = temp2;
  }
}

int main(int argc, char *argv[]) {
  Node *head = NULL;

  if (argc < 2) {
    printf("Usage: %s <list of integers>\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    int val = atoi(argv[i]);
    insertEnd(&head, val);
  }

  printf("Original list:\n");
  printList(head);

  zipList(&head);

  printf("Zipped list:\n");
  printList(head);
 
  // fill code here
  // Memory cleanup to prevent Valgrind memory leak errors
  Node *current = head;
  while (current != NULL) {
      Node *temp = current;
      current = current->next;
      free(temp);
  }

  return 0;
}
