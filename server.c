#include "socklib.h"
#include "serverlib.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define PORTNUM 13000
#define HOSTLEN 256
#define oops(msg) {perror(msg); exit(1);}

int server_requests;
time_t server_started;
int server_bytes_sent;

void* handle_call(void* fdptr);
void setup(pthread_attr_t* attrp);
FILE* p_open(const char* command, const char* mode) ;

int process_rq(int fd) {
 	char id[1000], pwd[1000], fname[1000], buf[1000];	
	FILE* fin;
	FILE* fout;
	int rq = -1;

	FILE* f;
	int c = 0, i = 0;
	char str[1000], cmd[1000];

	while(1){
		if((fin = fdopen(dup(fd), "r"))==NULL){
			perror("fin fdopen");
			exit(1);
		}
		if((fout = fdopen(dup(fd), "w"))==NULL){
			perror("fout fdopen");
			exit(1);
		}
		while(1){
			c = getc(fin);
			if ( c - '0'>=0 && c - '0' <=6){
				rq = c - '0';
				break;
			} 
		}
		printf("got a call on %d: request = %d\n",fd ,rq);
		printf("%d\n",rq);

		switch(rq){
			case SIGNUP :
				fscanf(fin, "%s %s",id, pwd);
				getc(fin);
				
				printf("%s %s\n",id ,pwd);
				
				if(sign_up(id,pwd) == 0){
					fprintf(fout, "%d", 0); // sign up failed 같은 아이디 존재
				}
				else{
					fprintf(fout, "%d",1); // sign up success
				}
				break;
			case LOGIN :
				fscanf(fin, "%s %s",id, pwd);
				getc(fin);
				printf("%s %s\n",id ,pwd);
				if(log_in(id, pwd) == 0){
					fprintf(fout, "%d", 0);
				}
				else{
					fprintf(fout, "%d",1);
				}
				break;
			case LOGOUT :
				fprintf(fout, "%d",1);
				fflush(fout);
				server_requests--;
				fclose(fout);
				fclose(fin);
				return 0;
				break;
			case LS:
				sprintf(cmd,"ls ./%s",id);
				
				f = popen(cmd, "r");
				
				if(fout != NULL && f != NULL){
					while((c=getc(f))!=EOF){
						putc(c,fout);
						fflush(fout);
						if(putc(c,stdout)=='\n')
							printf("\n");
					}
				}
				fprintf(fout,"\r\n");
				fflush(fout);
				pclose(f);
				break;
			case LOAD:
				fscanf(fin,"%s",fname);
				printf("%s",fname);
				c=getc(fin);
				sprintf(str,"./%s/%s",id,fname);

				if((f = fopen(str,"r"))==NULL){
					fprintf(fout, "%d",0); // dir에 파일이 존재하지 않을 때
					break;
				}else {
					fprintf(fout, "%d",1); // dir에 파일이 존재할 때
				} 
				printf("%s\n",str);
				
				if(fout != NULL && f != NULL){
					while((c=getc(f))!=EOF){
						putc(c,fout);
						fflush(fout);
						putc(c,stdout);
					}
				}
				fprintf(fout,"\r\n");
				fflush(fout);
				fclose(f);
				break;
			case SAVE:
				fscanf(fin,"%s",fname);
				getc(fin);
				sprintf(str,"./%s/%s",id,fname);
				
				if((f = fopen(str,"w"))==NULL){
					perror("save fopen");
					exit(1);
				}
				printf("%s\n",str);

				if(fin != NULL && f != NULL){
					while(c=getc(fin)){
						if (c == '\r') {
							if ((c =getc(fin)) == '\n') {
								break;
							}
						}
						putc(c,f);
						fflush(f);
					}
				}

				fclose(f);
				
				break;
			case DELETE:
				fscanf(fin,"%s",fname);
				getc(fin);
				sprintf(str,"./%s/%s",id,fname);
				if(remove(str) == -1){
					fprintf(fout,"%d",0);
				}else{
					fprintf(fout,"%d",1);
				}
				break;
		}
		fflush(fout);
		fclose(fout);
		strcpy(str,"");
		strcpy(cmd,"");
		strcpy(fname,"");
		c = 0;
		fclose(fin);
	}
	return 1;
}

int main(int argc, char* argv[]) {
	int sock, fd;
	int *fdptr;
	pthread_t worker;
	pthread_attr_t attr;

	if((sock = make_server_socket(PORTNUM)) == -1)
		oops("make_server_socket");
	
	setup(&attr);
	while (1) {
		fd = accept(sock, NULL, NULL);
		server_requests++;
		
		if (fd == -1) break;
		
		fdptr = malloc(sizeof(int));
		*fdptr = fd;
		printf("server requests %d\n",server_requests);
		pthread_create(&worker, &attr, handle_call, fdptr);

	}
}

void setup(pthread_attr_t* attrp)
{
	pthread_attr_init(attrp);
	pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED);

	time(&server_started);
	server_requests = 0;
	server_bytes_sent = 0;
}

void* handle_call(void* fdptr)
{
	int fd;
	
	fd = *(int*)fdptr;
	free(fdptr);
	
	process_rq(fd);
	
	return NULL;
}

FILE* p_open(const char* command, const char* mode) {
	int pip[2], pid;
	int child_end, parent_end;
		
	if (*mode == 'r') {
		parent_end =  0;
		child_end = 1;
	}
	
	if (*mode == 'w') {
		parent_end = 1;
		child_end = 0;
	}
	
	if (pipe(pip) == -1) {
		perror("pipe"); exit(1);
	}
	
	if ((pid = fork()) == -1) {
		perror("fork"); exit(1);
	}

	if (pid > 0) {
		if (close(pip[child_end]) == -1) { perror("close"); exit(1);}		
		return fdopen(pip[parent_end], mode);
	}

	if (pid == 0) {
		if (close(pip[parent_end]) == -1) {perror("close"); exit(1);}
		if (dup2(pip[child_end], child_end) == -1) {perror("dup2"); exit(1);}
		if (close(pip[child_end]) == -1) {perror("close"); exit(1);}
		execl("/bin/sh","sh", "-c", command, NULL);
		perror("execl");	
		exit(1);
	}
	perror("unknown");
	exit(1);
}