# Laurence Thompson
# CS 332, FA 2020

#To Compile: make
#To Run: ./job_scheduler <# of jobs run at once>

CC = gcc
CFLAGS = -Wall -g #replace -g with -O when not debugging

DEPS	= queue.h Makefile
OBJS	= queue.o job_scheduler.o

EXECS	= job_scheduler

all:	$(EXECS)

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

job_scheduler:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:	
	/bin/rm -i *.o $(EXECS)
