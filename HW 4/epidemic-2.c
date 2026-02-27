#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum TYPE {S, I, R};

// 1. The 2D-to-1D Hash Index
// We must map a 2D coordinate where x and y can be negative [-k, k] to a strictly positive integer.
// We shift the coordinates by adding 'k', then multiply by the grid width (2k + 1).
int idx(int x, int y, int k)
{
    return (x + k) + (y + k) * (2 * k + 1);
}

typedef struct Host
{
	int id;
	int x, y;
	int t;
	enum TYPE type;
} THost;

typedef struct node_tag {
   THost host;
   struct node_tag * next;
} node;

// 2. Node Creation (Heap Allocation)
node * create_node(THost host)
{
    // Malloc requests memory for exactly one node struct.
    node *new_node = (node *)malloc(sizeof(node));
    new_node->host = host;
    new_node->next = NULL; // ALWAYS initialize the next pointer to NULL to prevent wild pointers.
    return new_node;
}

// 3. Modifying the Head (Double Pointers)
void add_first(node **head, node *newnode)
{
    // Dereference the double pointer (*head) to access the actual list head.
    // The new node points to whatever the old head was pointing to.
    newnode->next = *head;
    
    // The actual head pointer is updated to point to the new node.
    *head = newnode;
}

node * remove_first(node **head)
{
    if (*head == NULL) return NULL; // Guard against empty lists
    
    // Save the node we are about to remove so we don't lose its memory address
    node *removed_node = *head;
    
    // Shift the head pointer to the second node in the list
    *head = (*head)->next;
    
    // Disconnect the removed node from the list completely
    removed_node->next = NULL;
    return removed_node;
}

// 4. Preventing Memory Leaks
void remove_all(node **head)
{
    // Keep removing the first node and FREEING it until the list is empty.
    while (*head != NULL) {
        node *temp = remove_first(head);
        free(temp); // CRITICAL: Free the heap memory or you will get a memory leak.
    }
}

// 5. Linked List Traversal
int location_match(node *head, THost host)
{
    node *curr = head; // Use a temporary pointer to walk the list so we don't lose the head.
    
    while (curr != NULL) {
        if (curr->host.x == host.x && curr->host.y == host.y) {
            return 1; // Match found
        }
        curr = curr->next; // Move to the next link in the chain
    }
    return 0; // No match found
}

// (Hash function included in starter code goes here)

// (Summary function included in starter code goes here)

int one_round(THost *hosts, int m, node *p_arr[], int n_arr, int k, int T)
{
    // Phase 1: State Updates
    for(int i = 0; i < m; i++)
    {
        if(hosts[i].type == S)
        {
            int index = hash(idx(hosts[i].x, hosts[i].y, k)) % n_arr;
            if(location_match(p_arr[index], hosts[i]))
            {
                // A Susceptible host shares a location with an Infected host in the hash table.
                hosts[i].type = I; // Become infected
                hosts[i].t = 0;    // Reset infection timer
            }
        }
		else if(hosts[i].type == I)
        {
            // Increment the time infected
            hosts[i].t++;
            
            // If the time infected reaches T, the host recovers.
            if (hosts[i].t >= T) {
                hosts[i].type = R;
            }
        }
    }

    // Phase 2: Hash Table Cleanup
    // We must completely wipe last round's Hash Table before the hosts move.
    for (int j = 0; j < n_arr; j++) {
        remove_all(&(p_arr[j])); // Pass the address of the array slot (a node **)
    }

    // Phase 3: Movement and Rebuilding the Hash Table
	for(int i = 0; i < m; i++)
	{
		int r = rand() % 4;
		
		// 0: up, 1: right, 2: down, 3: left
        // We MUST use 'break' in a switch statement, or it will execute every case.
		switch(r)
		{
			case 0: 
			    hosts[i].y++; 
			    if (hosts[i].y > k) hosts[i].y = -k; // Wrap-around logic
			    break;
			case 1: 
			    hosts[i].x++; 
			    if (hosts[i].x > k) hosts[i].x = -k;
			    break;
			case 2: 
			    hosts[i].y--; 
			    if (hosts[i].y < -k) hosts[i].y = k;
			    break;
			case 3: 
			    hosts[i].x--; 
			    if (hosts[i].x < -k) hosts[i].x = k;
			    break;
		}

		// Rebuild the hash table ONLY for hosts that are currently Infected.
		if(hosts[i].type == I)
		{
			node *r_node = create_node(hosts[i]); 
			int index = hash(idx(hosts[i].x, hosts[i].y, k)) % n_arr;
			add_first(&(p_arr[index]), r_node);
		}
	}

	return summary(hosts, m);
}

// (main() function from the starter code goes here)
