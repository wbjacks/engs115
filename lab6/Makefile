CC=mpicc
CFLAGS= -std=c99 -Wall -pedantic -ggdb
LOADERS=
EXE=p1

UTIL_CFILES=util/wkman.c util/pqueue.c util/queue.c
UTIL_HFILES=util/wkman.h util/pqueue.h util/queue.h util/macros.h
UTIL_OFILES=util/wkman.o util/pqueue.o util/queue.o

P1_CFILES=$(UTIL_CFILES) prob1/main.c
P1_HFILES=$(UTIL_HILES)  
P1_OFILES=$(UTIL_OFILES) prob1/main.o

OFILES=$(P1_OFILES)
HFILES=$(P1_HFILES)
CFILES=$(P1_CFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@ $(LOADERS)

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) -o $@ $(LOADERS)

clean:
	rm -f *~ .*.swp util/.*.swp core $(OFILES) $(EXE)
