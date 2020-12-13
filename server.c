#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define MAX_SEATS 30
#define MAXBUF 1024

void* handle_clnt(void * arg);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

typedef struct LoginData{
	int ClientFd;
	char ID[10];
	char PASSWORD[MAXBUF];
}LoginData;//클라이언트의 ID와 PASSWORD를 저장하는 구조체

void send_msg(int client, char * msg, int len);
int handle_rcvmsg(int clnt_sock, LoginData info);
int Login(LoginData * input);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;

	if(argc!=2) 
    {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutex1);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutex1);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s , clnt_sock=%d\n", inet_ntoa(clnt_adr.sin_addr), clnt_sock);
	}

	close(serv_sock);
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);

	return 0;
}
	
void *handle_clnt(void * arg)
{
	LoginData person;	
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
    char okay[MAXBUF] = "O";
    char no[MAXBUF] = "X";
	int select;
	char checkbuffer[MAXBUF]={0,};

	
    memset(&person, 0, sizeof(LoginData));

    printf("hello\n");

	str_len=read(clnt_sock, checkbuffer, sizeof(checkbuffer));
	if(strcmp(checkbuffer,"signin")==0)
	{
		select = 2;
	}
	if(strcmp(checkbuffer,"signup")==0)
	{
		select = 3;
	}

	if(select == 2)
	{
		printf("%d\n",select);
		str_len=read(clnt_sock, person.ID, sizeof(person.ID));
		//printf("%s",person.ID);
		
		str_len=read(clnt_sock, person.PASSWORD, sizeof(person.PASSWORD));
		//printf("%s",person.PASSWORD);
		
		if(str_len==-1)
		{
			error_handling("read() error!");
		}

		person.ClientFd = clnt_sock;

		if( (handle_rcvmsg(clnt_sock, person)) == 1)//okay
		{
			send_msg(clnt_sock,okay,1);
		}
		else//no
		{
			send_msg(clnt_sock,no,1);
		}
	}
	if(select == 3)
	{
		printf("%d\n",select);
		str_len=read(clnt_sock, person.ID, sizeof(person.ID));
		
		str_len=read(clnt_sock, person.PASSWORD, sizeof(person.PASSWORD));

		
		if(str_len==-1)
		{
			error_handling("read() error!");
		}

		person.ClientFd = clnt_sock;

		FILE* fp_signup;

		fp_signup = fopen("data.txt","a");
		fprintf(fp_signup,"\n");
		fprintf(fp_signup,"%s %s",person.ID,person.PASSWORD);
		fclose(fp_signup);
		send_msg(clnt_sock,okay,1);

	}
	
	
	pthread_mutex_lock(&mutex1);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			printf("clnt_sock=%d, i=%d\n", clnt_sock, i);
			while(i < clnt_cnt){
				clnt_socks[i]=clnt_socks[i+1];
				i++;
			}
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutex1);
	close(clnt_sock);
	return NULL;
}

int handle_rcvmsg(int clnt_sock, LoginData info)
{
	printf("handle_rcvmsg\n");
	FILE* fp;
	char loginData[MAXBUF];
	char* id;
	char* pw;
    int flag = 0;

	fp = fopen("data.txt","r");

	while(!feof(fp))
	{
		flag =0;
		printf("!!\n");
		fgets(loginData,sizeof(loginData),fp);

		printf("file ID PW: %s",loginData);

		char *temp = strtok(loginData," "); //공백을 기준으로 문자열 자르기
  
		if(strcmp(info.ID,temp)==0)
		{
			flag++;
			printf("%d\n",flag);
		}

		temp = strtok(NULL, " ");//널문자를 기준으로 다시 자르기

		if((strcmp(info.PASSWORD,temp)==0))
		{
			flag++;
			printf("%d\n",flag);
		}
		if(flag == 1 )
		{
			break;
		}
	}

    return flag;
}

void send_msg(int client, char * msg, int len)// send to all
{
	int i;
	pthread_mutex_lock(&mutex1);

	printf("send_msg: clnt_socks: %d connect!\n", client);
	write(client, msg, len);

	pthread_mutex_unlock(&mutex1);
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
