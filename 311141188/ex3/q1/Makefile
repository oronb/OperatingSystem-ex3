# Makefile for ex2-q1 winter 2020A
CFLAGS = -g -Wall
LDFLAGS = -lm # not really needed for this exercise

#CC = gcc -std=c99
CC = gcc -pthread
ECHO = echo "going to compile for target $@"
PROG1 = ex3_q1
PROGS = $(PROG1)

all: $(PROGS) test

$(PROG1): ex3_q1.o ex3_q1_given.o
	$(CC) $(CFLAGS) ex3_q1_given.o ex3_q1.o -o $(PROG1) $(LDFLAGS)

test: $(PROGS)
	./$(PROG1) > out.log 2> err.log

ex3_q1.o:  ex3_q1.h ex3_q1.c
	$(CC) $(CFLAGS) -c ex3_q1.c $(LDFLAGS)

ex3_q1_given.o:  ex3_q1_given.h ex3_q1_given.c
	$(CC) $(CFLAGS) -c ex3_q1_given.c $(LDFLAGS)

clean: 
	$(RM) *.o *~ $(PROGS) *.tmp *.log
