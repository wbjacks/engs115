CC=mpicc
CFLAGS=-std=c99 -Wall -pedantic -ggdb
LOADERS=-lm
EXE=p1 p2 p3 p4

UTIL_CFILES=util/wkman.c util/pqueue.c util/queue.c
UTIL_HFILES=$(UTIL_CFILES:.c:=.h) util/macros.h
UTIL_OFILES=$(UTIL_CFILES:.c:=.o)

P1_CFILES=$(UTIL_CFILES) prob1/main.c
P1_HFILES=$(UTIL_HILES)  
P1_OFILES=$(UTIL_OFILES) prob1/main.o

P2_CFILES=$(UTIL_CFILES) prob2/main.c
P2_HFILES=$(UTIL_HILES)  
P2_OFILES=$(UTIL_OFILES) prob2/main.o

P3_CFILES=$(UTIL_CFILES) prob3/main.c
P3_HFILES=$(UTIL_HILES)  
P3_OFILES=$(UTIL_OFILES) prob3/main.o

P4_CFILES=util/pqueue.c util/queue.c prob4/main.c prob4/wkman_optimized.c
P4_HFILES=$(P4_CFILES:.c:=.h) 
P4_OFILES=$(P4_CFILES:.c:=.o)

OFILES=$(P1_OFILES) $(P2_OFILES) $(P3_OFILES) $(P4_OFILES)
HFILES=$(P1_HFILES) $(P2_HFILES) $(P3_HFILES) $(P4_HFILES)
CFILES=$(P1_CFILES) $(P2_CFILES) $(P3_CFILES) $(P4_CFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@ $(LOADERS)

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) -o $@ $(LOADERS)

p2:	$(P2_OFILES) $(P2_HFILES)
	$(CC) $(CFLAGS) $(P2_OFILES) -o $@ $(LOADERS)

p3:	$(P3_OFILES) $(P3_HFILES)
	$(CC) $(CFLAGS) $(P3_OFILES) -o $@ $(LOADERS)

p4:	$(P4_OFILES) $(P4_HFILES)
	$(CC) $(CFLAGS) -pthread $(P4_OFILES) -o $@ $(LOADERS)

clean:
	rm -f *~ .*.swp util/.*.swp core *.csv $(OFILES) $(EXE)
