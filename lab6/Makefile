CC=mpicc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
LOADERS=-lm
EXE=p1 p2

UTIL_CFILES=util/wkman.c util/pqueue.c util/queue.c
UTIL_HFILES=$(UTIL_CFILES:.c:=.h) util/macros.h
UTIL_OFILES=$(UTIL_CFILES:.c:=.o)

P1_CFILES=$(UTIL_CFILES) prob1/main.c
P1_HFILES=$(UTIL_HILES)  
P1_OFILES=$(UTIL_OFILES) prob1/main.o

P2_CFILES=$(UTIL_CFILES) prob2/main.c
P2_HFILES=$(UTIL_HILES)  
P2_OFILES=$(UTIL_OFILES) prob2/main.o

OFILES=$(P1_OFILES) $(P2_OFILES)
HFILES=$(P1_HFILES) $(P2_HFILES)
CFILES=$(P1_CFILES) $(P2_CFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@ $(LOADERS)

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) -o $@ $(LOADERS)

p2:	$(P2_OFILES) $(P2_HFILES)
	$(CC) $(CFLAGS) $(P2_OFILES) -o $@ $(LOADERS)

clean:
	rm -f *~ .*.swp util/.*.swp core $(OFILES) $(EXE)
