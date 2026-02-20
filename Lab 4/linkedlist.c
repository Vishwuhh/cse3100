// Linked list demo program in CSE 3100
// The APIs may not be the best

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"

// print an error message by an error number, and return
// the function does not exit from the program
// the function does not return a value
void error_message(enum ErrorNumber errno) {
  char *messages[] = {
      "OK",
      "Memory allocaton failed.",
      "Deleting a node is not supported.",
      "The number is not on the list.",
      "Sorting is not supported.",
      "Reversing is not supported.",
      "Token is too long.",
      "A number should be specified after character d, a, or p.",
      "Token is not recognized.",
      "Invalid error number."};

  if (errno < 0 || errno > ERR_END)
    errno = ERR_END;
  printf("linkedlist: %s\n", messages[errno]);
}

node *new_node(int v) {
  node *p = malloc(sizeof(node)); // Allocate memory
  if (p == NULL) {
    error_message(ERR_NOMEM);
    exit(-1);
  }

  // Set the value in the node.
  p->v = v; // you could do (*p).v
  p->next = NULL;
  return p; // return
}

node *prepend(node *head, node *newnode) {
  newnode->next = head;
  return newnode;
}

node *find_node(node *head, int v) {
  while (head != NULL) {
    if (head->v == v)
      return head;
    head = head->next;
  }
  return head;
}

node *find_last(node *head) {
  if (head != NULL) {
    while (head->next != NULL)
      head = head->next;
  }
  return head;
}

node *append(node *head, node *newnode) {
  node *p = find_last(head);

  newnode->next = NULL;
  if (p == NULL)
    return newnode;
  p->next = newnode;
  return head;
}

node *delete_list(node *head) {
  while (head != NULL) {
    node *p = head->next;
    free(head);
    head = p;
  }
  return head;
}

void print_list(node *head) {
  printf("[");
  while (head) {
    printf("%d, ", head->v);
    head = head->next;
  }
  printf("]\n");
}

void print_node(node *p) {
    printf("%p: v=%-5d next=%p\n", p, p->v, p->next);
}

void print_list_details(node *head) {
  while (head) {
    print_node(head);
    head = head->next;
  }
}

// functions that have not been implemented

node *delete_node(node *head, int v) {
  // If the list is empty, the value cannot be found.
  if (head == NULL) {
    error_message(ERR_NOTFOUND);
    return head;
  }

  // Case 1: The node to be deleted is the head node.
  if (head->v == v) {
    node *temp = head;
    head = head->next;
    free(temp);
    return head;
  }

  // Case 2: The node to be deleted is somewhere else in the list.
  node *current = head;
  while (current->next != NULL) {
    if (current->next->v == v) {
      node *temp = current->next;
      current->next = current->next->next;
      free(temp);
      return head;
    }
    current = current->next;
  }

  // Case 3: We traversed the whole list and didn't find the value.
  error_message(ERR_NOTFOUND);
  return head;
}

/*
 * Given a pointer to the head node of an acyclic list, change the
 * next links such that the nodes are linked in reverse order.
 * Allocating new nodes or copying values from one node to another
 * is not allowed, but you may use additional pointer variables.
 * Return value is a pointer to the new head node.
 */
node *reverse_list(node *head) {
  node *prev = NULL;
  node *current = head;
  node *next = NULL;

  while (current != NULL) {
    // Store the next node before overwriting the current node's pointer
    next = current->next;
    
    // Reverse the link
    current->next = prev;
    
    // Move the prev and current pointers one step forward
    prev = current;
    current = next;
  }
  
  // 'prev' will be pointing to the new head of the reversed list
  return prev;
}
