CC=gcc
CFLAGS= -std=c99 -Wall -pedantic -ggdb -pthread -Ofast
LOADERS=-lm
EXE=p1 p2 p5

UTIL_CFILES=util/integrate.c util/generator.c util/queue.c
UTIL_HFILES=util/integrate.h util/generator.h util/queue.h
UTIL_OFILES=util/integrate.o util/generator.o util/queue.o

P1_CFILES=$(UTIL_CFILES) prob1/main.c
P1_HFILES=$(UTIL_HILES)  
P1_OFILES=$(UTIL_OFILES) prob1/main.o

P2_CFILES=$(UTIL_CFILES) prob2/main.c
P2_HFILES=$(UTIL_HFILES)
P2_OFILES=$(UTIL_OFILES) prob2/main.o

P5_CFILES=$(UTIL_CFILES) prob5/main.c
P5_HFILES=$(UTIL_HFILES)
P5_OFILES=$(UTIL_OFILES) prob5/main.o

OFILES=$(P1_OFILES) $(P2_OFILES) $(P5_OFILES)
HFILES=$(P1_HFILES) $(P2_HFILES) $(P5_HFILES)
CFILES=$(P1_CFILES) $(P2_CFILES) $(P5_CFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@ $(LOADERS)

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) -o $@ $(LOADERS)

p2:	$(P2_OFILES) $(P2_HFILES)
	$(CC) $(CFLAGS) $(P2_OFILES) -o $@ $(LOADERS)

p5: $(P5_OFILES) $(P5_HFILES)
	$(CC) $(CFLAGS) $(P5_OFILES) -o $@ $(LOADERS)

clean:
	rm -f *~ .*.swp util/.*.swp core $(OFILES) $(EXE)
