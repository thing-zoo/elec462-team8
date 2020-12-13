SERVERNAME = server
CLIENTNAME = client
.PHONY: clean

RM = rm -f

$(SERVERNAME): server.c socklib.o serverlib.o
	gcc -o $(SERVERNAME) $(SERVERNAME).c socklib.o serverlib.o -lpthread -g

$(CLIENTNAME): $(CLIENTNAME).c socklib.o
	gcc -o $(CLIENTNAME) $(CLIENTNAME).c socklib.o -g

socklib.o: socklib.c socklib.h
	gcc -Wall -c socklib.c -g

serverlib.o: serverlib.c serverlib.h
	gcc -Wall -c serverlib.c -g

clean:
	$(RM) *.o
