#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// (The TPlayer and TProperty structs go here)

int transaction(TPlayer *p1, TPlayer *p2, int amount)
{
	// 1. Struct Pointers
	// p1 and p2 are POINTERS to the players. We MUST use the arrow operator (->) to access balances.
	
	if (p1->balance >= amount) {
		// Player 1 has enough money. Normal transaction.
		p1->balance -= amount;
		p2->balance += amount;
		return 1; // Return 1 for success
	} else {
		// Player 1 is bankrupt! They forfeit all remaining balance to Player 2.
		p2->balance += p1->balance;
		p1->balance = 0;
		return 0; // Return 0 for failure (game over)
	}
}

int one_round(int m, int n, TPlayer p[], TProperty prop[])
{
	for(int i = 0; i < m; i++)
	{
		int steps = rand() % 6 + 1 + rand() % 6 + 1;
		
		// 1. Cyclic Movement
		// The board wraps around from n-1 back to 0.
		int old_loc = p[i].loc;
		int new_loc = (old_loc + steps) % n; // Modulo 'n' forces it to wrap around.
		
		// 2. Pass 'GO' Logic
		// Because the board only goes one direction, if the new location is numerically smaller 
		// than the old location, the player must have wrapped past index 0.
		if (new_loc < old_loc) {
			p[i].balance += n; // Award passing GO money
		}
		
		p[i].loc = new_loc; // Update the player's physical location
		
		// 3. Property Logic
		if (prop[new_loc].owner_id == -1) {
			// The property is unowned. The current player claims it.
			prop[new_loc].owner_id = p[i].id;
			
		} else if (prop[new_loc].owner_id != p[i].id) {
			// The property is owned by someone else. Rent must be paid!
			int owner = prop[new_loc].owner_id;
			int rent = prop[new_loc].rent;
			
			// Call our transaction function. 
			// Notice we must use '&' to pass the ADDRESS of the specific players in the array!
			int success = transaction(&p[i], &p[owner], rent);
			
			if (success == 0) {
				// The player went bankrupt. The prompt says if one player cannot pay, 
				// the function should return 0 and the rest of the players lose their turn.
				return 0; 
			}
		}
	}
	// If the loop finishes without anyone going bankrupt, the round was successful.
	return 1;
}

// (The print_result, monopoly, and main functions from the starter code go here)
