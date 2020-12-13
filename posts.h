#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


#define MALLOC(p,s) if(!(p = malloc(s))) { fprintf(stderr,"malloc error\n"); exit(1); }
#define REALLOC(p,s) if(!(p = realloc(p,s))) { fprintf(stderr,"realloc error\n"); exit(1); }
#define TITLEBUF 32
#define CONTBUF 1024

struct post{
	char ID[10];
	char title[TITLEBUF];
	char contents[CONTBUF];
	char time[30];
	int year;
	int month;
	int day;
};

static char clnt_ID[10] = "John";


int write_title(struct post* p);
void get_time(struct post* p);
void write_contents(struct post* p);
void store_post(struct post p);
void empty_day(int year,int month, int day);
void print_posts(struct post *plist, int len, int year, int month, int day);
void posts_list(int clnt_sock,int year, int month,int day);
void print_notice(void);
void write_post(int year, int month, int day);
void print_post(struct post p);
int delete_post(struct post p);
void print_posts_set(struct post *plist, int st, int end, int no);
struct post * post_refresh(int *sum, int year, int month, int day);
int select_delete(int st, int end);

#include "posts.c"
