#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBUF 1024
#define MAXCLIENT 100//max client num
#define ALL 1

void error_handling(char *message);
void writeMessage(void* client_message, void* num, int basefd, int maxfd);

typedef struct LoginData{
	char ID[MAXBUF];
	char PASSWORD[MAXBUF];
}LoginData;//클라이언트의 ID와 PASSWORD를 저장하는 구조체

typedef struct Who{
	int ClientFd;
	char ID[MAXBUF];
}Who;//서버에 접속한 클라이언트의 파일디스크립터와 ID저장 -> 구분

LoginData Data[MAXCLIENT];//기존 사용자들의 ID와 PASSWORD를 저장하는 객체
LoginData ClientData;//client ID와 PASSWORD를 저장하는 객체
Who WhoLogin[MAXCLIENT];//서버에접속하는 클라이언트의 정보를 저장하는 객체

int main(int argc, char *argv[])
{
	int optval = 1;//소켓옵션의 설정값     
	
	int serv_sock;
	int clnt_sock;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	
	socklen_t clnt_addr_size;
	int clen,data_len;                                  
	fd_set read_fds,tmp_fds;//디스크립터 셋트(단일 비트 테이블)
	int fd;
	int index,maxfd;

	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR, &optval,sizeof(optval));//소켓 옵션을 설정

	memset(WhoLogin,0,sizeof(WhoLogin));
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);

	//FD_SET 디스크립터의 세팅
	FD_ZERO(&read_fds);
	FD_SET(serv_sock,&read_fds);
	maxfd= serv_sock;


	while(1)
	{
	//fd_set디스크립터 테이블은 일회성. 그렇기 때문에 해당값을 미리 옮겨 놓고 시작해야 한다. 그렇기 때문에 복사를 먼져 하고 시작해야 한다. 
		tmp_fds=read_fds;
		//인터페이스 상에서 디바이스에 들어온 입력에 대한 즉각적인 대응이 필요.
		if(select(maxfd+1,&tmp_fds,0,0,(struct timeval *)0)<1){
			perror("select error : ");
			exit(1);
		}

		for(fd=0;fd<maxfd+1;fd++)
		{
			if(FD_ISSET(fd,&tmp_fds))
			{
				if(fd==serv_sock)
				{ 
					if((clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr,&clen))<0)
					{
						perror("accept error : ");
						exit(0);
					}

					FD_SET(clnt_sock,&read_fds);
					printf("새로운 클라이언트 %d번 파일 디스크립터 접속\n",clnt_sock);

					for(index=0;index<MAXCLIENT;index++)
					{
						if(WhoLogin[index].ClientFd==0)
						{
							WhoLogin[index].ClientFd=clnt_sock;
							maxfd++;
							break;
						}
					}
					if(clnt_sock>maxfd)
					{
						maxfd = clnt_sock;
					}
				}
				else
				{ 
					memset(Data,0,sizeof(Data));
					//클라이언트로부터 ID,PW를 수신받는다.
					data_len = read(fd,(struct message*)&ClientData,sizeof(ClientData));

					//클라이언트로부터 ID,PW 들어왔다면 유효한 정보인지 확인 
					if(data_len>0)
					{
						int flag = 0;
						FILE* fp;
						char* id;
						char* pw;

						fp = fopen("data.txt","r");

						while(!feof(fp))
						{
							fscanf(fp,"%s %s",id,pw);
							strcpy(Data[clnt_sock].ID,id);
							strcpy(Data[clnt_sock].PASSWORD,pw);

							if(strcmp(ClientData.ID,Data[clnt_sock].ID)==0)
							{
								if(strcmp(ClientData.PASSWORD,Data[clnt_sock].PASSWORD)==0)
								{
									flag=1;
								}
							}
						}

						if(flag == 1)
						{
							printf("Hello %s :D \nWelcome to our program\n",id);
						}
						else
						{
							printf("Opps please again\n");
							exit(1);
						}
						
						

					}
					else if(data_len==0)
					{
						for(index=0;index<MAXCLIENT;index++)
						{
							if(WhoLogin[index].ClientFd==fd)
							{
								WhoLogin[index].ClientFd=0;
								strcpy(WhoLogin[index].ID,"");
								break;
							}
						}

						close(fd);

						FD_CLR(fd,&read_fds);

						if(maxfd==fd)
							maxfd--;

						printf("클라이언트 %d번 파일 디스크립터 해제\n",fd);



					}else if(data_len<0){

						perror("read error : ");

						exit(1);

					}

					

				}

			}

		}

	}

	return 0;

}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}