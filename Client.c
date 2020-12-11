/* 
 * =============================================================================
 * SystemProgrammingProject
 * =============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <curses.h>

//init
#define TICKS_TO_COOLDOWN 0
int inputCoolCount = 0;

int key = 0;
void update();
//

//처음 옵셥
#define QUIT 0
#define MAIN_MENU 1
#define START 2
#define SIGNIN 3
#define SIGNUP 4
#define DIARY 5
//


//화면 만들기..
#define MAX_CHOICE_CHARS 30 // max number of chars a menu option can have
#define TOP_BANNER_PAD 2
#define BANNER_OPTIONS_PAD 4
//

//Server&Client
#define MAXBUF 1024
#define MAXCLIENT 100//max client num
//

//첫 화면에 옵션 선택부분
#define MAX_CHOICE_CHARS 30 // max number of chars a menu option can have
#define TOP_BANNER_PAD 2
#define BANNER_OPTIONS_PAD 4

typedef struct
{
	int stateID;
	char text[MAX_CHOICE_CHARS];
} menuChoice;

menuChoice *mc;

int numChoices, currentNumChoices = 0;
int titleColumns, titleRows; // number of colums and rows in the title banner
int cursorPos; // current row cursor is on (vertical cursor position)
int topOptPos, botOptPos; // position of the first menu choice, last menu choice

char *titleText; // char pointer to hold text of the tile
//

typedef struct LoginData{
	char ID[MAXBUF];
	char PASSWORD[MAXBUF];
}LoginData;//클라이언트의 ID와 PASSWORD를 저장하는 구조체

//Screen
#define MAX_FPS 50
#define MSEC_IN_SEC 1000
#define DELAY (MSEC_IN_SEC / MAX_FPS)
#define TICS_PER_SEC MAX_FPS
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
int colors[] = {0,1,2,3,4,5,6,7};

//state
int state = 1;
int prevState =0;
int substate = 0;
//

char titleBanner[] = {"\
0000  00000    0    0000  0    0\
0   0   0     0 0   0   0  0  0 \
0   0   0    0   0  0 00    00  \
0   0   0    00000  0   0   00  \
0000  00000 0     0 0   0   00  "};

WINDOW *w;


//main.c
void initialize();
void render();
void gamePause();
void cleanup();
int getKey();
bool notReadyToQuit();
void update();
int tick = 0;



//오류 다루기
void error_handling(char *message);

//처음 옵셥 선택할 때 사용되는 함수들
int getState();
int getPrevState();
int setState(int newState);


/* 
 * =======================
 *첫 화면에 옵션 선택부분 구현
 * =======================
 */


// call first to allocate mem for memory choices
void initMenu(int numberOfChoices);

// initiate the banner, coulums is width of banner, rows is number 
// of rows, *text is a character array holding the banner
// returns true if successful, false if rows or colums is too big
// assert that colums is no greater than width of screen
// rows no greater than height of screen
bool initTitle(int columns, int rows, char *text);

// id is the state id associated with the choice, choicetext is the actual
// text of the choice
// returns -1 if you try to initiate more than numberOfChoices
int initMenuChoice(int id, char choiceText[]);

// called by update(), passes key pressed from keyboard (if any)
void updateMainMenu(int key);

// called by render()
void renderMainMenu();

// call before quitting, frees up allocated memory
void cleanupMenu();



//signin
void updateSignup(key);




int main(int argc, char* argv[])
{
	/*int sock;
	struct sockaddr_in serv_addr;
	char message[30];
	int str_len;
    LoginData input;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&input,0,sizeof(input));
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

    printf("ID PASSWORD:");
    scanf("%s %s",input.ID,input.PASSWORD);

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");


	
	if(write(sock,(struct message*)&input,sizeof(input))<0)
    {
		error_handling("write error : ");
	}

	close(sock);
	return 0;*/

	initialize();

	while(notReadyToQuit())
	{
		update();
		render();
		gamePause();

		tick++;
		if(tick == TICS_PER_SEC)
		{
			tick = 0;
		}
	}

	cleanup();

	return 0;

}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void initialize()
{
	w = initscr(); // init a curses window

	start_color(); // call so can use colors
	int i, j, k = 0; // counter variables
	for(i = 0; i < 8; i++) // initialize all 64 color pairs
	{
		for(j = 0; j < 8; j++)
		{
			if(k) // skip cp #1; already defined as black and white
			{
				init_pair(k, colors[i], colors[j]);
			}
			k++;
		}
	}

	/*
	 * These two lines make it so that getch() doesn't need to wait 
	 * for a character to be entered for getch() to return.
	 * This allows the game to update and render even when there is no input
	 * Also put as: call getch() without blocking
	 */
	cbreak(); // put terminal in c break mode
	nodelay(w, TRUE); // don't wait for enter to read char

	noecho(); // turn off key echoing
	keypad(w, TRUE); // allow getch() to detect non-character key presses

	curs_set(0); // hide the cursor

	initMenu(3);
	
	initTitle(32, 5, titleBanner);
	initMenuChoice(SIGNIN, "Sign in");
	initMenuChoice(SIGNUP, "Sign up");
	initMenuChoice(QUIT, "Quit");

	//initGame();

	setState(MAIN_MENU);
}


void render() 
{
	clear();
	switch(getState())
	{
		case MAIN_MENU:
			renderMainMenu();
			break;
		// case SIGNIN:
		// 	renderSignin();
		// 	break;
		// case SIGNUP:
		// 	renderSignup();
		// 	break;

		default:
			break;
	}
	refresh(); // call curses's refresh funct to update screen
}

bool notReadyToQuit()
{
	if(state)
	{
		return TRUE;
	} else 
	{
		return FALSE;
	}
}



//update.c

void update() {

	key = getKey();
	if(key != ERR){
		if(inputCoolCount == 0)
		{
			inputCoolCount = TICKS_TO_COOLDOWN;
		} 
		else {
			key = ERR;
		}

		switch(getState())
		{
			case MAIN_MENU:
				updateMainMenu(key);
				break;
			// case SIGNUP:
			// 	updateSignup(key);
			// 	break;	
			// case SIGNIN:
			// 	updateSignin(key);
			// 	break;	
			// default:
			// 	break;
		}
	} 

	if(inputCoolCount > 0)
	{
			inputCoolCount--;
	}
}

int getKey(){
	int key = getch();
	return key;
}


void gamePause() 
{
	napms(DELAY); // from curses.h, sleeps for n milliseconds
}

void cleanup()
{
	curs_set(1); // set cursor back to being visible
	cleanupMenu();
	//cleanupGame();
	clear();
	refresh();
	delwin(w);
    	endwin();
}


//state.c
int getState()
{
	return state;
}

int getPrevState()
{
	return prevState;
}

int setState(int newState)
{
	prevState = state;
	state = newState;
}


//mainMenu.c
void initMenu(int numberOfChoices)
{
	// allocate mem for menu choices
	mc = (menuChoice*) malloc(numberOfChoices * sizeof(menuChoice));
	numChoices = numberOfChoices;
}

bool initTitle(int columns, int rows, char *text)
{
	// make sure entire menu will actually fit
	if((columns > SCREEN_WIDTH)||(rows > (SCREEN_HEIGHT - 1 - numChoices)))
	{
		return FALSE;
	}
	titleColumns = columns;
	titleRows = rows;

	// allocate memory for title banner's text
	titleText = malloc(rows * columns + 1);


	// copy text into title text
	int i;
	for(i = 0; i < rows * columns; i++)
	{
		titleText[i] = text[i];
	}

	cursorPos = TOP_BANNER_PAD + titleRows + BANNER_OPTIONS_PAD + 1;
	topOptPos = cursorPos;
	botOptPos = cursorPos + numChoices - 1;
	return TRUE;
}

int initMenuChoice(int id, char choiceText[])
{
	// make sure it's not going to initialize more options than memory 
	// was allocated for choices
	if(currentNumChoices >= numChoices)
	{
		return -1;
	}

	mc[currentNumChoices].stateID = id;
	strcpy(mc[currentNumChoices].text, choiceText);

	return ++currentNumChoices;
}

void updateMainMenu(int key)
{
	// process keyboard input
	switch(key)
	{
		case KEY_UP:
			if(cursorPos > topOptPos)
			{
				cursorPos--; // move cursor up one
			}
			break;
		case KEY_DOWN:
			if(cursorPos < botOptPos)
			{
				cursorPos++; // move cursor down one
			}
			break;
		case '\n':
		case 'z':
			// if enter pressed, set the state of the choice that
			// was selected
			setState(mc[cursorPos - topOptPos].stateID);
			break;
		case 27:
			setState(QUIT);
			break;
		default:
			break;
	}
}

void renderMainMenu()
{
	// perform setup for printing title banner
	int leftEdgePos = (int) (80 - titleColumns)/2;
	int x = leftEdgePos;
	int y = TOP_BANNER_PAD;
	int i;

	// print out title banner
	for(i = 0; i < titleColumns * titleRows; i++)
	{
		mvaddch(y, x, titleText[i]);
		if((x - leftEdgePos) >= (titleColumns-1))
		{
			x = leftEdgePos;
			y++;
		} else x++;
	}

	// move cursor to correct row to print out choices
	y = TOP_BANNER_PAD + titleRows + BANNER_OPTIONS_PAD;

	// print out choices
	for(i = 0; i < currentNumChoices; i++)
	{
		y++;
		leftEdgePos = (int) (80 - strlen(mc[i].text))/2;
		// add cursors to indicate slected choice
		if(y == cursorPos)
		{
			mvaddstr(y, leftEdgePos - 3, "-> ");
			mvaddstr(y, leftEdgePos + strlen(mc[i].text), " <-");
		}
		mvaddstr(y, leftEdgePos, mc[i].text);
	}
	mvprintw(22,0, "Use arrow keys to select an option"); 
	mvprintw(23,0, "Select an option with (Enter)"); 
	mvprintw(24,0, "Select \"Quit\" or press (ESC) to quit"); 
}

void cleanupMenu()
{
	free(mc);
	free(titleText);
}

void updateSignup(key)
{
// process keyboard input
	switch(key)
	{
		case KEY_UP:
			if(cursorPos > topOptPos)
			{
				cursorPos--; // move cursor up one
			}
			break;
		case KEY_DOWN:
			if(cursorPos < botOptPos)
			{
				cursorPos++; // move cursor down one
			}
			break;
		case '\n':
		case 'z':
			// if enter pressed, set the state of the choice that
			// was selected
			setState(mc[cursorPos - topOptPos].stateID);
			break;
		case 27:
			setState(QUIT);
			break;
		default:
			break;
	}
}


