//  socklib.h
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>

#define 	SIGNUP		0
#define 	LOGIN		1
#define 	LOGOUT		2
#define 	SAVE		3
#define		LOAD		4
#define		LS		    5
#define		DELETE		6
#define     DEFAULT     7
#define     MENU        8

int make_server_socket(int);
int make_server_socket_q(int, int);
int connect_to_server(char*, int);

