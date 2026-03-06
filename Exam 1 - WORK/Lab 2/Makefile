# This is a comment line

#define variables so it is easier to make changes
CC=gcc
CFLAGS=-g -Wall -std=c99
# Updated to include both targets [cite: 12]
TARGETS=ex-factorial catalan

all: $(TARGETS)

ex-factorial: ex-factorial.c
	$(CC) $(CFLAGS) -o ex-factorial ex-factorial.c

# New rule for catalan [cite: 11]
catalan: catalan.c
	$(CC) $(CFLAGS) -o catalan catalan.c

clean:
	rm -rf *.o *~ $(TARGETS) a.out

