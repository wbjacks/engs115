CC=gcc
CFLAGS= -ansi -Wall -pedantic

CFILES=queue.c shop.c
HFILES=queue.h
OFILES=queue.o shop.o

all:	shop

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

shop:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) -o shop


clean:	
	rm -f *~ shop

