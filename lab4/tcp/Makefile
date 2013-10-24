CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic -ggdb -pthread
LOADERS= 
EXE=chat_client chat_server

UTIL_CFILES= util/user.c util/queue.c util/pqueue.c
UTIL_HFILES= util/user.h util/queue.h util/pqueue.h
UTIL_OFILES= util/user.o util/queue.o util/pqueue.o

CLIENT_CFILES=$(UTIL_CFILES) client/client.c
CLIENT_HFILES=$(UTIL_HILES)  client/client.h 
CLIENT_OFILES=$(UTIL_OFILES) client/client.o

SERVER_CFILES=$(UTIL_CFILES) server/server.c server/threads.c server/commands.c
SERVER_HFILES=$(UTIL_HFILES) server/server.h server/threads.h server/commands.h
SERVER_OFILES=$(UTIL_OFILES) server/server.o server/threads.o server/commands.o

OFILES=$(CLIENT_OFILES) $(SERVER_OFILES)
HFILES=$(CLIENT_HFILES) $(SERVER_HFILES)
CFILES=$(CLIENT_CFILES) $(SERVER_CFILES)

all:	$(EXE)

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@ $(LOADERS)

chat_client:	$(CLIENT_OFILES) $(CLIENT_HFILES)
	$(CC) $(CFLAGS) $(CLIENT_OFILES) -o $@ $(LOADERS)

chat_server:	$(SERVER_OFILES) $(SERVER_HFILES)
	$(CC) $(CFLAGS) $(SERVER_OFILES) -o $@ $(LOADERS)

clean:
	rm -f *~ .*.swp util/.*.swp core $(OFILES) $(EXE)
