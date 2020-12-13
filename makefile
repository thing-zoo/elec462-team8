## Makefile for webserv

.PHONY: clean

RM = rm -f

all: server client

server: server.o
	gcc -o server server.c

client: client.o
	gcc -o client client.c

server.o: server.c
	gcc -Wall -g -c server.c

client.o: client.c
	gcc -Wall -g -c client.c

clean:
	$(RM) *.o
	