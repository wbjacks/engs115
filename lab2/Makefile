CC=gcc
CFLAGS= -std=c99 -Wall -pedantic -ggdb -lm -pthread
EXE=p1 p2

UTIL_CFILES=util/integrate.c
UTIL_HFILES=util/integrate.h
UTIL_OFILES=util/integrate.o

P1_CFILES=$(UTIL_CFILES) prob1/main.c
P1_HFILES=$(UTIL_HILES)  
P1_OFILES=$(UTIL_OFILES) prob1/main.o

P2_CFILES=$(UTIL_CFILES) prob2/main.c
P2_HFILES=$(UTIL_HFILES)
P2_OFILES=$(UTIL_OFILES) prob2/main.o

OFILES=$(P1_OFILES) $(P2_OFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) -o $@

p2:	$(P2_OFILES) $(P2_HFILES)
	$(CC) $(CFLAGS) $(P2_OFILES) -o $@

clean:
	rm -f *~ .*.swp util/.*.swp $(OFILES) $(EXE)
