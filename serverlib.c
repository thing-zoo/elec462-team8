#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

struct client_if{
	char id[100];
	char pswd[100];
};

int sign_up(char*, char*); //sign up
int check(char*); //check id already exist 
int log_in(char*, char*); //log in
void mkDir(char*);

int sign_up(char *id, char *pw){
	int fd;
	int flag;
	char* fname="client.txt";
   	struct client_if client;
	/*
	get id pswd
	*/
	
	if(check(id)==1) return 0; // check id already exist 

	strcpy(client.id,id); // input id pw
	strcpy(client.pswd,pw);


	//write file
	if ((fd=open(fname,O_RDWR|O_CREAT,0644))==-1){
		printf("fopen err"); exit(0);
	}
	if((flag=fcntl(fd,F_GETFL))==-1){
		perror("fcntlfailed");
		exit(1);
	}
	flag |=O_APPEND;
	if ((fcntl(fd, F_SETFL, flag)) == -1) {
	    perror("fcntl failed");
	    exit(1);
	}
	write(fd,(struct client_if*)&client,sizeof(client));

	close(fd);
	mkDir(id);
	return 1;
}

int check(char* id){//check id already exist -> exist 1 : not 0
	int ch=0;
	struct client_if client;
	FILE* f;
	char* fname="client.txt";
	if ((f=fopen(fname,"r"))==NULL){
		if((f=fopen(fname,"w"))==NULL){
			return -1;
		}else{
			return 0;
		}
	}

	while(!feof(f)){
		if (1 != fread(&client,sizeof(struct client_if),1,f)) {
		
		}
		if(!strcmp(client.id,id)){
			printf("이미 있는 아이디 입니다.\n");
			ch=1;
			break;
		}
	}
	fclose(f);
	return ch;
}

int log_in(char *id, char *pswd){
	char* fname="client.txt";
	FILE* f;
    struct client_if client;
	
	if ((f=fopen(fname,"r"))==NULL){
		return 0;
	}
	while(!feof(f)){
		if (1 != fread(&client,sizeof(struct client_if),1,f)) {
		
		}
		if (!strcmp(id,client.id) && !strcmp(pswd,client.pswd)){
			fclose(f);
			return 1;	
		}
	}
	printf("로그인 실패\n");
	fclose(f);
	return 0;
}

void mkDir( char* id ){
		int nResult = mkdir( id,0755 );
			if( nResult == 0 ){
				printf( "폴더 생성 성공" );
			}
			else if( nResult == -1 ){
				perror( "폴더 생성 실패 - 폴더가 이미 있거나 부정확함\n" );
			}
}
