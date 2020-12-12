#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>

char* week[7] = { "SUN", "MON","TUE","WED","THU","FRI","SAT" };
int monthdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

//example
char* text[] = {
        "System Programming Team 8   ",
        "----------2020--10----------",
        " SUN MON TUE WED THU FRI SAT",
        "                   1   2   3",
        " 4  5  6  7  8  9 10",
        "11 12 13 14 15 16 17",
        "18 19 20 21 22 23 24",
        "25 26 27 28 29 30 31",
        "[Q] quit [<-] previous month [space bar] today [->] next month"
    };

typedef struct date {
    int year;
    int month;
    int day;
}tdate;

void drawCal(struct tm t, int monthdays);
void IsLeapYear(int year);
tdate today;//fix today
struct tm *t;

int main(int argc, char *argv[])
{
    time_t now = time(NULL);
    t = localtime(&now);

    today.year = t->tm_year + 1900;
    today.month = t->tm_mon + 1;
    today.day = t->tm_mday;

    IsLeapYear(t->tm_year+1900);
    
    initscr();
    curs_set(0); // hide the cursor
    keypad(stdscr, TRUE);
    
    while(1)
    {
        clear();
        drawCal(*t, monthdays[t->tm_mon]);
        refresh();

        switch (getch())
        {
        case KEY_UP://previous month
            //go to last day of previous month
            now -= 86400 * t->tm_mday;
            t = localtime(&now);
            //go to first day of previous month
            now -= 86400 * (monthdays[t->tm_mon]-1);
            t = localtime(&now);
            IsLeapYear(t->tm_year+1900);
            break;
        case KEY_DOWN://next month
            //go to first day of next month
            now += 86400 * (monthdays[t->tm_mon] - t->tm_mday + 1);
            t = localtime(&now);
            IsLeapYear(t->tm_year+1900);
            break;
        case KEY_LEFT://previous day
            now -= 86400 ;
            t = localtime(&now);
            IsLeapYear(t->tm_year+1900);
            break;
        case KEY_RIGHT://next day
            now += 86400 ;
            t = localtime(&now);
            IsLeapYear(t->tm_year+1900);
            break;
        case KEY_HOME://back today
            //back to real
            break;
        default:
            break;
        }
    }
    
    //getch();
    //endwin();
    
    return 0;
}

void IsLeapYear(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        monthdays[1] = 29;
    else
        monthdays[1] = 28;
}

void drawCal(struct tm t, int monthdays)
{
    int firstday = t.tm_wday;//day of week on 1st each month
    char yearmonth[30];//----------2020--10----------
    int dayOfWeek;//0-6: sun~sat
    
    int i = t.tm_mday;
    while(i != 1)
    {
        i--;
        if (--firstday == -1)
            firstday = 6;
    }
    dayOfWeek = firstday;

    move(0, 0);
    addstr("System Programming Team 8   ");
    
    move(1, 0);
    sprintf(yearmonth, "----------%4d--%2d----------", t.tm_year + 1900, t.tm_mon + 1);
    addstr(yearmonth);

    move(2,0);
    addstr(" SUN MON TUE WED THU FRI SAT");

    move(3, 0);
    for (int i = 0; i < firstday; i++)
        printw("%4s", "");
    for (int i = 0, day = 1, y = 4; i < monthdays; i++)
    {
        printw("%2s", "");

        //color weekend
        if (dayOfWeek == 0)//sun
            attron(COLOR_PAIR(1));
        if (dayOfWeek == 6)//sat
            attron(COLOR_PAIR(2));
        
        //highlight today    
        if((t.tm_year + 1900) == today.year && day == today.day && (t.tm_mon + 1) == today.month)
            attron(A_STANDOUT);
        
        //blink selected day
        if(day == t.tm_mday)
            attron(A_BLINK);
            
        printw("%2d", day);
        attrset(A_NORMAL);
        
        day++; 
        dayOfWeek++;

        if (dayOfWeek == 7)
        {//go to nextweek
            addstr("\n");
            //move(y++, 0);
            dayOfWeek = 0;
        }
    }
}