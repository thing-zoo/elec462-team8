
.PHONY: clean

RM = rm -f

all: server client

server: server.o
	gcc -o server server.o -lcurses -lpthread

client: client.o
	gcc -o client client.o -lcurses

server.o: server.c
	gcc -Wall -g -c server.c

client.o: client.c
	gcc -Wall -g -c client.c

clean:
	$(RM) *.o
	