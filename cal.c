
char* week[7] = { "SUN", "MON","TUE","WED","THU","FRI","SAT" };
int monthdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

typedef struct date {
    int year;
    int month;
    int day;
}tdate;

void drawCal(struct tm t, int monthdays);
void IsLeapYear(int year);
tdate today;//fix today
struct tm *t;

void cal(int sock)
{
    time_t now = time(NULL);
    time_t realtime = time(NULL);
    t = localtime(&now);

    today.year = t->tm_year + 1900;
    today.month = t->tm_mon + 1;
    today.day = t->tm_mday;

    IsLeapYear(t->tm_year+1900);


    while(1)
    {
        clear();
        drawCal(*t, monthdays[t->tm_mon]);
        refresh();
        int op = getch();

        switch (op)
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
            now = realtime;
            t = localtime(&now);
            IsLeapYear(t->tm_year+1900);
            break;
        case '\n'://go to post
            posts_list(t->tm_year+1900,t->tm_mon+1,t->tm_mday); //input year, month, day
            break;
        case 'Q'://exit    
        case 'q':
            endwin();
            int inst = 999;
            write(sock, (int*)&inst, sizeof(inst));
            close(sock);
            
            exit(0);
        default:
            break;
        }
    }
    
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
    char yearmonth[100];//----------2020--10----------
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
    
    move(2, 0);
    sprintf(yearmonth, "------------------------%4d--%2d------------------------", t.tm_year + 1900, t.tm_mon + 1);
    addstr(yearmonth);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    
    attron(COLOR_PAIR(1));

    move(4,0);
    for (int i = 0; i < 7; i++)
    {
        if (i == 0)
            attron(COLOR_PAIR(2));
        else if (i == 6)
            attron(COLOR_PAIR(3));
        else
            attron(COLOR_PAIR(1));

        addstr("     ");
        addstr(week[i]);
        
    }

    move(6, 0);
    for (int i = 0; i < firstday; i++)
        printw("%8s", "");
    for (int i = 0, day = 1, y = 4; i < monthdays; i++)
    {
        printw("%6s", "");

        //color weekend
        if (dayOfWeek == 0)//sun
            attron(COLOR_PAIR(2));
        else if (dayOfWeek == 6)//sat
            attron(COLOR_PAIR(3));
        else
            attron(COLOR_PAIR(1));
        
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
            addstr("\n\n");
            //move(y++, 0);
            dayOfWeek = 0;
        }
    }
    addstr("\n\n");
    addstr("[up] previous month         ");
    addstr("[down] next month\n");
    addstr("[left] previous day         ");
    addstr("[right] next day\n");
    addstr("[enter] go to post          ");
    addstr("[home] back to today\n");
    addstr("[q] quit");
}