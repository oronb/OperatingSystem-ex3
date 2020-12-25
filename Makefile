# Makefile for ex2-q1 winter 2020A
CFLAGS = -g -Wall
LDFLAGS = -lm # not really needed for this exercise

#CC = gcc -std=c99
CC = gcc
ECHO = echo "going to compile for target $@"
PROG1 = ex3_q1
PROGS = $(PROG1)

all: $(PROGS) test

$(PROG1): ex3_q1.o ex3_q1_given.o
	$(CC) $(CFLAGS) ex3_q1_given.o ex3_q1.o -o $(PROG1)

test: $(PROGS)
	# make sure you have input file in.txt.
	# so the following command works
	./$(PROG1)

ex3_q1.o:  ex3_q1.h ex3_q1.c
	$(CC) $(CFLAGS) -c ex3_q1.c

ex3_q1_given.o:  ex3_q1_given.h ex3_q1_given.c
	$(CC) $(CFLAGS) -c ex3_q1_given.c

clean: 
	$(RM) *.o *~ $(PROGS) *.tmp