


#include <stdio.h>

static int monDays[] = {31,  28,  31,   30,  31,  30,  31,  31,  30,  31,  30,  31};
static int totDays[] = {0,   31,  59,   90, 120, 151, 181, 212, 243, 273, 304, 334};
static int cent[] = {
    1000, 1100, 1200, 1300, 1400,
    1500, 1600, 1700, 1800, 1900,
    2000, 2100, 2200, 2300, 2400,
    2500, 2600, 2700, 2800, 2900,
    3000, 3100, 3200};
static int centSerial[] = {
    -328716, -292192, -255668, -219143, -182619,
    -146095, -109571, -73046, -36522,       2,
    36526, 73051, 109575, 146099, 182623,
    219148, 255672, 292196, 328720, 365245,
    401769, 438293, 474817};

static int isCentLeap[] = {
    1, 1, 1, 1, 1,
    1, 1, 0, 0, 0,
    1, 0, 0, 0, 1,
    0, 0, 0, 1, 0,
    0, 0, 1};



static int daysInMonth(int year, int month)
{
    int days = monDays[month];
    if (month == 1) { // february is special
        int whichCent = 22;
        while ( whichCent > 0 && cent[ whichCent ] > year )
            whichCent--;
        if ( ( year == cent[ whichCent ] && isCentLeap[ whichCent ] ) ||
             ( year != cent[ whichCent ] && year % 4 == 0 ) ) {
            days++;
        }
    } // endif
    return days;
}


double getDateSerial(int year,int month,int day,int hour,int minute,int second,int millisecond) {
    double serial = 0;
    int s;
    int daysOfYear; // this val is used to overcome a compiler-bug
    int whichCent;

    // take care of the negative and overly positive values
    while (millisecond < 0) {
        millisecond += 1000;
        second--;
    } // endwhile
    while (millisecond > 999) {
        millisecond -= 1000;
        second++;
    } // endwhile
    while (second < 0) {
        second += 60;
        minute--;
    } // endwhile
    while (second > 59) {
        second -= 60;
        minute++;
    } // endwhile
    while (minute < 0) {
        hour--;
        minute += 60;
    } // endwhile
    while (minute > 59) {
        hour++;
        minute -= 60;
    } // endwhile
    while (hour < 0) {
        day--;
        hour += 24;
    } // endif
    while (hour > 23) {
        day++;
        hour -= 24;
    } // endif

    while (month < 0) {
        year--;
        month += 12;
    } // endwhile
    while (month > 11) {
        year++;
        month -= 12;
    } // endwhile

    while (day < 0) {
        if (month == 0) {
            month = 11;
            year--;
            day += daysInMonth(year,month);
        } else {
            month--;
            day += daysInMonth(year,month);
        }
    } // endwhile
    while (day > daysInMonth(year,month)) {
        if (month == 11) {
            day -= daysInMonth(year,month);
            month = 0;
            year++;
        } else {
            day -= daysInMonth(year,month);
            month++;
        } // endif
    } // endwhile

    s = hour;
    s = s * 60 + minute;
    s = s * 60 + second;
    serial = s;
    serial = serial*1000.0 + (double)millisecond;
    serial /= (double)86400000.0;

    whichCent = 22;
    while ( whichCent > 0 && cent[ whichCent ] > year )
        whichCent--;

    // determine the base century
    if ( cent[ whichCent ] > year ) {
        // date lower than valid range...
        s = centSerial[ 0 ];
    } else {
        // base serial value from lookup table...
        s = centSerial[ whichCent ];

        // if we're not on the base year, do a special case
        if ( year > cent[ whichCent ] ) {
            int numLeapYears;

            // get how many days have been in all the skipped years
            daysOfYear = ( year  - cent[ whichCent ] ) * 365;
            s = s + daysOfYear;

            // add in the number of leap-years, excluding the century
            numLeapYears = ( ( year -1 ) - cent[ whichCent ] )/4;
            s += numLeapYears;

            // add in the leap year for the century, if applicable
            if ( isCentLeap[ whichCent ] )
                s++;
        }

        // if we're on a leap-year, and the month is after feb, add one
        // more
        if ( ( year == cent[ whichCent ] && isCentLeap[ whichCent ] ) ||
             ( year != cent[ whichCent ] && year % 4 == 0 ) ) {
            if ( month >= 2 ) { // are we on March or later?
                s++;
            }
        }

        s += day;
        s += totDays[ month ];
    }
    serial += s;
    return serial;
}

    
int main (int argc, char *argv[])
{
    //year is the actual year
    //month is 0-Jan, 1-Fed, 2-Mar, etc...
    //day is 0-first day of month, etc...
    printf("Now is: %lf\n",getDateSerial(2000,8,25,0,0,0,0));
}
