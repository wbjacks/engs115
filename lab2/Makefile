CC=gcc
CFLAGS= -std=c99 -Wall -pedantic -ggdb -lm
EXE=p1
CMAIN=main.c
HMAIN=

P1_CFILES=util/integrate.c $(CMAIN)
P1_HFILES=util/integrate.h $(HMAIN)
P1_OFILES=util/integrate.o

OFILES=$(P1_OFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

p1:	$(P1_OFILES) $(P1_HFILES)
	$(CC) $(CFLAGS) $(P1_OFILES) $(CMAIN) -o $@

clean:
	rm -f *~ .*.swp util/.*.swp $(OFILES) $(EXE)
