CC=gcc
CFLAGS= -ansi -Wall -pedantic -g

CFILES=queue.c shop.c
HFILES=queue.h shop.h
OFILES=queue.o shop.o
UTEST_CFILES = queue.c unit.c
UTEST_OFILES = queue.o

all:	shop unit

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

shop:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) -o shop

unit:	$(UTEST_OFILES)
	$(CC) $(CFLAGS) $(UTEST_OFILES) unit.c -o unit

clean:	
	rm -f *~ *.o shop unit
