#include "socklib.h"
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#define oops(msg) {perror(msg);exit(1);}
#define MAX_STRING 100

int c;

void prompt(int type) {
    
    switch (type) {
        case SIGNUP:
            break;

        case LOGIN:
            break;

        case LOGOUT:
            break;

        case SAVE:
            break;

        case LOAD:
            break;

        case LS:
            break;

        case DELETE:
            break;

        case DEFAULT:
            printf("CHOOSE MENU:\n");
            printf("1. Login\t2. SignUp\n");
            break;
        case MENU:
            printf("CHOOSE MENU:\n");
            printf("1. SAVE\t2. LOAD\t3. LS\n4. DELETE\t5. LOGOUT\n");
            break;
        default:
            printf("TRY AGAIN! YOU'VE SELECTETD : %d\n", type);
            break;
    }
    return;
}

int typeOf(char* input, int promptType) {
    int type = -1;
    
    type = atoi(input);
    
    switch (promptType) {
        case DEFAULT:
            switch (type) {
                case 1:
                    type = LOGIN;
                    break;
                case 2:
                    type = SIGNUP;
                    break;
                default:
                    perror("typeOf : type error");
                    break;
            }
        break;
        case MENU:
            switch (type) {
                case 1:
                    type = SAVE;
                    break;
                case 2:
                    type = LOAD;
                    break;
                case 3:
                    type = LS;
                    break;
                case 4:
                    type = DELETE;
                    break;
                case 5:
                    type = LOGOUT;
                    break;
                default:
                    perror("typeOf : menu type error");
                break;
            }
         break;
        default:
            perror("typeOf : prompt type error");
            exit(1);
            break;
    }
    
    return type;
}

char* getUserInput() {
    char* input = (char*)malloc(sizeof(char)*MAX_STRING);
    int n;
    n = scanf("%s", input);
    if (n > MAX_STRING) { perror("input_max overflow"); exit(1);}
    //
    
    return input;
}

void tty_mode(int how){
	static struct termios original_mode;
	static int original_flags;
	if(how ==0){
		tcgetattr(0,&original_mode);
		original_flags = fcntl(0, F_GETFL);
	}
	else{
		tcsetattr(0,TCSANOW,&original_mode);
		fcntl(0,F_SETFL,original_flags);
	}
}

void set_cr_noecho_mode(){
	struct termios ttystate;
	tcgetattr(0,&ttystate);
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN]=1;
	tcsetattr(0,TCSANOW,&ttystate);
}

char* getUserPassword() {
	tty_mode(0);
	set_cr_noecho_mode();
    char* input = (char*)malloc(sizeof(char)*MAX_STRING);
    int i=0;
	int c;
    
    getc(stdin);
	for(i=0;i<MAX_STRING;i++){
		c=getc(stdin);
        if(c=='\n') {
            if (i == 0) {
                perror("\n front");
                exit(1);
            }
            break;
        }
        input[i]=c;
        printf("*");
    
	}
	input[i]='\0';
    if (i == MAX_STRING) { perror("input_max overflow"); exit(1);}
	tty_mode(1);
    printf("\n");
    return input;
}

int signup(int fd) {
    //
    char* id;
    char* pwd;
    int signUpSuccess = 0;
    FILE* fin, *fout;
    
    prompt(SIGNUP);
    printf("id: ");
     id = getUserInput();
    printf("password: ");
     pwd = getUserPassword();
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("signup fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("signup fdopen"); exit(1);}
    
    fprintf(fout, "%d %s %s ", SIGNUP, id, pwd);
    fflush(fout);
    
    while(1) {
        c = getc(fin);
        if (c == '0' || c == '1') { signUpSuccess = c -'0'; break;}
    }
    
    if (signUpSuccess) {
        printf("signUp Success\n");
    } else {
        printf("signUp Failed\n");
    }
    
    fflush(fout);
    fclose(fout);
    
    
    fclose(fin);
    
    return signUpSuccess;
}

int loginS(int fd) {
    char* id;
    char* pwd;
    int loginSuccess = 0;
    FILE* fin, *fout;
    
    prompt(LOGIN);
    printf("id: ");
     id = getUserInput();
    printf("password: ");
     pwd = getUserPassword();
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("login fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("login fdopen"); exit(1);}
    
    fprintf(fout, "%d %s %s ", LOGIN, id, pwd);
    fflush(fout);
    
    while(1) {
         c = getc(fin);
         if (c == '0' || c == '1') { loginSuccess = c -'0'; break;}
     }
    
    if (loginSuccess) {
        printf("login Success\n");
    } else {
        printf("login Failed\n");
    }
    fclose(fout);
    fclose(fin);
    return loginSuccess;
}

void save(int fd) {
    char* fileName;
    int saveSuccess = 0;
    int c;
    FILE* fin, *fout, *file;
       
    prompt(SAVE);
    printf("file name:\n");
    fileName = getUserInput();
    
    if ((file = fopen(fileName, "r")) == NULL) {perror("save fopen"); return;}
    if ((fout = fdopen(fd, "w")) == NULL) {perror("save fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("save fdopen"); exit(1);}
    
    fprintf(fout, "%d %s ", SAVE, fileName);
    
    if(fout!=NULL && file!=NULL){
        while((c=fgetc(file))!= EOF){
            fputc(c,fout);
        }
        fprintf(fout, "\r\n");
        fflush(fout);
        fclose(fout);
        fclose(file);
        saveSuccess = 1;
    }

    if (saveSuccess) {
        printf("save Success\n");
    } else {
        printf("save Failed\n");
    }
    fclose(fout);
    fclose(fin);
}

void load(int fd) {
    char* fileName;
    int fileExist = 0;
    int loadSuccess = 0;
    int c;
    FILE* fin, *fout, *file;
       
    prompt(LOAD);

    printf("file name:\n");
    fileName = getUserInput();
       
    if ((file = fopen(fileName, "w")) == NULL) {perror("load fopen"); exit(1);}
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("load fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("load fdopen"); exit(1);}
    
    fprintf(fout, "%d %s ", LOAD, fileName);
    fflush(fout);
    
    while(1) {
        c = getc(fin);
        if (c == '0' || c == '1') { fileExist = c -'0'; break;}
    }

    
    if (!fileExist) {
        printf("file not exist on server. : %s\n", fileName);
        return;
    }
    
    if(fin!=NULL && file!=NULL){
        while((c=getc(fin))!=EOF){
            if (c == '\r') {
                if ((c=getc(fin)) == '\n') {
                    break;
                }
            }
            putc(c,file);
        }
        fclose(fin);
        fclose(file);
        loadSuccess = 1;
    }
    
    if (loadSuccess) {
        printf("load Success\n");
    } else {
        printf("load Failed\n");
    }
    fclose(fout);
    fclose(fin);
}
void ls(int fd) {
    int lsSuccess = 0;
    int c;
    FILE* fin, *fout;
       
    prompt(LS);
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("load fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("load fdopen"); exit(1);}
    
    fprintf(fout, "%d ", LS);
    fflush(fout);
    
    if(fin!=NULL){
        while((c=getc(fin))!=EOF){
            if (c == '\r') {
                if ((c=getc(fin)) == '\n') {
                    break;
                }
            }
            putc(c,stdout);
        }
        fclose(fin);
        
        lsSuccess = 1;
    }
    if (lsSuccess) {
        printf("ls Success\n");
    } else {
        printf("ls Failed\n");
    }
    fclose(fout);
    fclose(fin);
}
void del(int fd) {
    int deleteSuccess = 0;
    FILE* fin, *fout;
    char* fileName;
    
    prompt(DELETE);
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("logout fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("logout fdopen"); exit(1);}
    
    printf("file name:\n");
    fileName = getUserInput();
    fprintf(fout, "%d %s ", DELETE, fileName);
    fflush(fout);
    
    while(1) {
        c = getc(fin);
        if (c == '0' || c == '1') { deleteSuccess = c -'0'; break;}
    }
    
    
    if (deleteSuccess) {
        printf("delete Success\n");
    } else {
        printf("delete Failed\n");
    }
    fclose(fout);
    fclose(fin);
}

int logoutS(int fd) {
    int logoutSuccess = 0;
    FILE* fin, *fout;
    
    prompt(LOGOUT);
    
    if ((fout = fdopen(fd, "w")) == NULL) {perror("logout fdopen"); exit(1);}
    if ((fin = fdopen(fd, "r")) == NULL) {perror("logout fdopen"); exit(1);}
    
    fprintf(fout, "%d ", LOGOUT);
    fflush(fout);
    
    while(1) {
        c = getc(fin);
        if (c == '0' || c == '1') { logoutSuccess = c -'0'; break;}
    }
    
    if (logoutSuccess) {
        printf("logout Success\n");
    } else {
        printf("logout Failed\n");
    }
    fclose(fout);
    fclose(fin);
    return logoutSuccess;
}

void talk_with_server(int fd) {
    char* input;
    int type;
    // quit value
    int quit = 0;
    while (!quit) {
        prompt(DEFAULT);
        input = getUserInput();
        type = typeOf(input, DEFAULT);
        // login fisrt
        switch (type) {
            case SIGNUP:
                signup(dup(fd));
                break;
            case LOGIN:
                quit = loginS(dup(fd));
                break;
            default:
                printf("try again. you put : %s", input);
            break;
        }
    }
    
    //reset quit
    quit = 0;
    while (!quit) {
        // select menu
        prompt(MENU);
        input = getUserInput();
        type = typeOf(input, MENU);
        switch (type) {
            case LOGOUT:
                prompt(LOGOUT);
                quit = logoutS(dup(fd));
            break;

            case SAVE:
                prompt(SAVE);
                save(dup(fd));
            break;

            case LOAD:
                prompt(LOAD);
                load(dup(fd));
            break;

            case LS:
                prompt(LS);
                ls(dup(fd));
            break;

            case DELETE:
                prompt(DELETE);
                del(dup(fd));
            break;

            default:
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    int fd;
    if (argc != 2) {
        fprintf(stderr, "put > client 192.168.43.236");
        exit(1);
    }
    
    fd = connect_to_server(argv[1], 13000);
    if (fd == -1)    exit(1);
    talk_with_server(fd);
    close(fd);
}
