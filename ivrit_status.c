#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>

int interval = 1;

/*
 *returns 1 if discharging
 */
int is_discharging() {
    char charging_status[12];
    FILE *fp = fopen("/sys/class/power_supply/BAT0/status", "r");
    fgets(charging_status, 12, fp);
    fclose(fp);
    if (strcmp(charging_status, "Discharging") == 0) {
        return 1;
    }
}

/*
 *writes output of a bash command to a buffer
 */
const char * get_command(char* buffer, int buffersize, char* command) {
    char buf[256];
    FILE *f = popen(command, "r");
    fgets(buf, sizeof(buf), f);
    pclose(f);
    strncpy(buffer, buf, buffersize-1);
    buffer[buffersize-1] = '\0';
}

/*
 *sets the X server root window text
 */
int setxroot(char* status) {
    Display * dpy = NULL;
    Window win = 0;
    size_t length = 0;
    ssize_t bytes_read = 0;
    dpy = XOpenDisplay(getenv("DISPLAY"));
    if (dpy == NULL)
    {
            fprintf(stderr, "Can't open display, exiting.\n");
            exit(1);
    }
    win = DefaultRootWindow(dpy);
    XStoreName(dpy, win, status);
    XFlush(dpy);
}

int main()
{
    while (1 == 1){
        char status[256];
        status[0] = '\0';

        /*month of year*/
        char month_of_year[256];
        get_command(month_of_year, sizeof(month_of_year), "date +\"%m\" | tr -d \"\\t\\n\\r\"");
        int month_num = atoi(month_of_year);
        /*these are typed backwards on purpose*/
        static char* hebrew_months[] = {
            "ראוני" ,
            "ראורבפ" ,
            "צרמ" ,
            "לירפא" ,
            "יאמ" ,
            "ינוי" ,
            "ילוי" ,
            "טסוגןא" ,
            "רבמטפס" ,
            "רבוטקוא" ,
            "רבמבונ" ,
            "רבמצד"
        };
        strcat(status, hebrew_months[month_num - 1]);
        strcat(status, "ב" );

        /*gap*/
        strcat(status, " ");

        /*day of month*/
        char day_of_month[3];
        get_command(day_of_month, sizeof(day_of_month), "date +\"%d\" | tr -d \"\\t\\n\\r\"");
        int day_num = atoi(day_of_month);
        char day_ordinal[256];
        day_ordinal[0] = '\0';
        char ones_ordinal[256];
        /*these are typed backwards on purpose*/
        static char* ones_ordinals[] = {
            "ןושאר" , //first
            "ינש" , //second
            "ישילש" , //third
            "יעיבר" , //fourth
            "ישימח" , //fifth
            "ישש" , //sixth
            "יעיבש" , //seventh
            "ינימש" , //eighth
            "ינישת" , //ninth
            "ירישע" //tenth
        };
        static char* ones_cardinals[] =  {
            "דחא" , //one
            "מינש" , //two
            "השלש" , //three
            "העברא" , //four
            "השימח" , //five
            "השש" , //six
            "העבש" , //seven
            "הנומש" , //eight
            "העשת" , //nine
        };
        if (day_num < 11) {
            strcat(day_ordinal, ones_ordinals[day_num - 1]);
        } else if (day_num > 10 && day_num < 20) {
            strcat(day_ordinal, "רשע" );
            strcat(day_ordinal, " ");
            strcat(day_ordinal, ones_cardinals[day_num - 11]);
        } else if (day_num == 20) {
            strcat(day_ordinal, "מירשע" );
        } else if (day_num > 20 && day_num < 30) {
            strcat(day_ordinal, ones_cardinals[day_num - 21]);
            strcat(day_ordinal, "ו" );
            strcat(day_ordinal, " ");
            strcat(day_ordinal, "מירשע" );
        } else if (day_num == 30) {
            strcat(day_ordinal, "מישולש" );
        } else {
            strcat(day_ordinal, ones_cardinals[day_num - 31]);
            strcat(day_ordinal, "ו" );
            strcat(day_ordinal, " ");
            strcat(day_ordinal, "מישולש" );
        }
        strcat(status, day_ordinal);

        /*gap*/
        strcat(status, " ,");

        /*day of week*/
        char day_of_week[256];
        get_command(day_of_week, sizeof(day_of_week), "date +\"%w\" | tr -d \"\\t\\n\\r\"");
        day_num = atoi(day_of_week);
        /*these are typed backwards on purpose*/
        static char* hebrew_days[] = {
            "ןושאר" ,
            "ינש" ,
            "ישילש" ,
            "יעיבר" ,
            "ישימח" ,
            "ישיש" ,
            "תבש"
        };
        strcat(status, hebrew_days[day_num]);
        if (day_num < 7) {
            strcat(status, " " );
            strcat(status, "מוי" );
        };

        /*split separator*/
        strcat(status, ";");

        /*battery percentage*/
        char batt_percent[4];
        get_command(batt_percent, sizeof(batt_percent), "cat /sys/class/power_supply/BAT0/capacity | tr -d \"\\t\\n\\r\"");
        if (is_discharging() == 1) {
            strcat(status, batt_percent);
            strcat(status, "🜄");
        } else if (strcmp(batt_percent, "100") == 0) {
            /*do nothing*/
            strcat(status, "✡");
        } else {
            strcat(status, batt_percent);
            strcat(status, "🜂");
        }

        /*gap*/
        strcat(status, " ");

        /*volume*/
        char volume[4];
        get_command(volume, sizeof(volume), "amixer get Master | grep -Eo \"\\w+%\" | head -n 1 | tr -d \"\\t\\n\\r\"");
        strcat(status, volume);

        /*gap*/
        strcat(status, " ");

        /*time*/
        char time[256];
        get_command(time, sizeof(time), "date +\"%H:%M:%S\" | tr -d \"\\t\\n\\r\"");
        strcat(status, time);

        /*output and sleep*/
        fprintf(stderr, "%s", status);
        setxroot(status);
        sleep(interval);
    }
}
