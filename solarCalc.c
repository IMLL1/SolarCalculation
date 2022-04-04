/**
 * todo:
 **/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXDECIMALS 6
#define DEGCODE 248

#define DEG2RAD (M_PI / 180)
#define RAD2DEG (180 / M_PI)

#define LONGRANGE 180
#define LATRANGE 90

#define DAYSINYEAR 365
#define DAYSINMONTH 28
#define DAYSMEDMONTH 30
#define DAYSLONGMONTH 31
#define NUMMONTHS 12
#define JAN 1
#define FEB 2
#define MAR 3
#define APR 4
#define MAY 5
#define JUN 6
#define JUL 7
#define AUG 8
#define SEP 9
#define OCT 10
#define NOV 11
#define DEC 12

#define JDATE2000 2451544.5
#define HRSINDAY 24
#define MININHR 60
#define HRS12HR 12

#define JULCENTURY 36525
#define LOCTIME 0.5
#define BEGINDAY 0
#define ENDDAY (1 - (1 / (HRSINDAY * MININHR)))

#define TWILIGHTANGLE -0.833

// function declarations

// math functions

double cosd(double);
double sind(double);
double tand(double);
double acosd(double);
double asind(double);
int numDecimals(double);

// input functions

void getCoords(double *, double *);
double getTimeZone(void);
void getDate(int *, int *, int *);
int getMonth(void);
int goAgain(void);

// time functions

int isLeapYear(int);
int monthLen(int, int);
int getDay(int, int);
int cumDaysInYr(int, int, int);
int cumLpYrs(int);
int cumDays(int, int, int);
double calcJDate(int, int, int, double);
void calcDate(double, double, int *, int *, int *);
double roundToMin(double);
int hours(double);
int minutes(double);
int hours12Hr(double);
char hrType12Hr(double);

// sunrise/sunset functions

double calcEventApprox(double, double, double, double, double, int);
double calcEvent(double, double, double, double, int);
double calcEventDay(double, double, double, double, int);

// output functions

void printCoords(double, double);
void printDate(double);
void dispTime(double);
void nominalOutput(double, double, double, double);
void extremeLatOutput(double, double, double, double, int);
void printAll(double, double, double, double);

int main(void)
{
    double latitude;  // latitude (deg, - is south, + is north)
    double longitude; // longitude (deg, - is west, + is east)
    double timeZone;  // time zone in UTC offset.
    int year;         // years in standard calendar
    int month;        // month number (1-indexed)
    int day;          // day number of the month (1-indexed)
    double jDate;     // Julian date
    int iterate;      // whether to keep iterating

    printf("\n\n\tSolar Calculations");
    do
    {
        printf("\n\n");

        // get position
        getCoords(&longitude, &latitude);
        timeZone = getTimeZone();

        // get time
        getDate(&year, &month, &day);

        jDate = calcJDate(day, month, year, timeZone);

        printAll(jDate, timeZone, longitude, latitude);

        // prompt to go again
        iterate = goAgain();
    } while (iterate == 1);

    return 0;
}

// MATH FUNCTIONS

/**
 * Cosine function in degrees
 *
 *  Inputs:
 * angleDeg: the angle, in degrees. Domain of all real numbers
 *
 *  Output:
 * The value of the cosine, between -1 and +1 inclusive of both
 **/
double cosd(double angleDeg)
{
    return cos(angleDeg * DEG2RAD);
}

/**
 * Sine function in degrees
 *
 *  Inputs:
 * angleDeg: the angle, in degrees. Domain of all real numbers
 *
 *  Output:
 * The value of the sine, between -1 and +1 inclusive of both
 **/
double sind(double angleDeg)
{
    return sin(angleDeg * DEG2RAD);
}

/**
 * Tangent function in degrees
 *
 *  Inputs:
 * angleDeg: the angle, in degrees. Domain of all real numbers excluding the vertical angles
 *
 *  Output:
 * The value of the tangent, domain of all real numbers
 **/
double tand(double angleDeg)
{
    return tan(angleDeg * DEG2RAD);
}

/**
 * Arccosine written to work in degrees
 *
 *  Inputs:
 * ratio: the cosine of an unknown angle (adjacent / hypotenuse). Must be between -1 and 1.
 *
 *  Output:
 * The angle in degrees. The output will be between 0 and 180 degrees.
 **/
double acosd(double ratio)
{
    return (acos(ratio) * RAD2DEG);
}

/**
 * Arcsine written to work in degrees
 *
 *  Inputs:
 * ratio: the sine of an unknown angle (opposite / hypotenuse). Must be between -1 and 1.
 *
 *  Output:
 * The angle in degrees. The output will be between -90 and 90 degrees.
 **/
double asind(double ratio)
{
    return (asin(ratio) * RAD2DEG);
}

/**
 * Calculates the number of nonzero decimal places that a number has
 *
 *  Inputs:
 * number: the number to check
 *
 *  Output:
 * The angle, in degrees. Range: (-180, 180]
 **/
int numDecimals(double number)
{
    int decimals = 0; // the number of decimals

    while (fmod(number, 1) != 0 && decimals < MAXDECIMALS)
    {
        number *= 10;
        number = fmod(number, 1);
        decimals++;
    }

    return decimals;
}

// USER INPUT FUNCTIONS

/**
 * Prompts the user whether to iterate the program again (validates input as upper or lower case Y/N
 * )
 *
 *  Inputs:
 * None
 *
 *  Output:
 * Binary whether to loop again
 **/
int goAgain(void)
{
    char userInput;     // what the user inputted
    int scanInputs = 0; // number of scanf inputs

    printf("\n\n\n");
    do
    {
        fflush(stdin);
        printf("Calculate again [Y/N]? ");
        scanInputs = scanf("%c", &userInput);
        userInput = toupper(userInput);
    } while (scanInputs != 1 && !(userInput == 'Y' || userInput == 'N'));

    return userInput == 'Y' ? 1 : 0;
}

/**
 * Prompts the user to enter coordinates, and validates the input. Longitude must have magnitude no greater than 180. Latitude must be between -90 and 90.
 *
 *  Inputs:
 * longitude pointer: variable in which to store user-entered longitude. Positive values are east, negative are west.
 * latitude pointer: variable in which to store latitude. Positive values are north, negative are south.
 *
 *  Output:
 * None
 **/
void getCoords(double *longitude, double *latitude)
{
    int scanInputs;        // number of scanf inputs
    char inputStr[BUFSIZ]; // input string
    int dummy;             // dummy variable to check correct number of inputs

    do
    {
        fflush(stdin);
        printf("Enter coordinates (latitutde longitude): ");
        fgets(inputStr, BUFSIZ, stdin);
        scanInputs = sscanf(inputStr, "%lf %lf %d", latitude, longitude, &dummy);

        // errors
        if (scanInputs != 2)
        {
            printf("\tInvalid input! Enter exactly two coordinates, separated by a space.");
            printf("\n\tExample input:\n\tEnter coordinates (latitutde longitude): 51.51 -0.13\n");
        }
        else if (fabs(*latitude) >= LATRANGE || fabs(*longitude) >= LONGRANGE)
        {
            printf("\tInvalid input! Latitude goes from -90 to 90, longitude from -180 to 180.");
            printf("\n\tExample input:\n\tEnter coordinates (latitutde longitude): 47.6 -122.3\n");
        }
    } while (scanInputs != 2 || fabs(*latitude) >= LATRANGE || fabs(*longitude) >= LONGRANGE);
}

/**
 * Prompts the user to enter a time zone and validates it. Must be between UTC-13 and UTC+13, and must be an integer time zone.
 *
 *  Inputs:
 * None
 *
 *  Output:
 * Time zone UTC offset
 **/
double getTimeZone(void)
{
    double timeZoneOffset; // time zone UTC offset
    int scanInputs;        // number of scanf inputs
    char inputStr[BUFSIZ]; // input string
    int dummy;             // dummy variable to check correct number of inputs

    do
    {
        fflush(stdin);
        printf("Time zone: UTC");
        fgets(inputStr, BUFSIZ, stdin);
        scanInputs = sscanf(inputStr, "%lf %d", &timeZoneOffset, &dummy);
        if (scanInputs != 1)
        {
            printf("\tInvalid input! Enter exactly one input");
            printf("\n\tExample input:\n\tUTC+0\n");
        }
        else if (fabs(timeZoneOffset) > 13)
        {
            printf("\tInvalid input! Enter a time zone in UTC (GMT) offset. Must be between -13 and +13.");
            printf("\n\tExample input:\n\tUTC-8\n");
        }
    } while (scanInputs != 1 || fabs(timeZoneOffset) > 13);

    return timeZoneOffset;
}

/**
 * Prompts the user to enter a date as YYYY DD MM
 *
 *  Inputs:
 * int pointer year: the variable to store the year in
 * int pointer month: the variable to store the month in
 * int pointer day: the variable to store the day in
 *
 *  Output:
 * None
 **/
void getDate(int *year, int *month, int *day)
{
    int scanInputs;        // number of scan inputs
    char inputStr[BUFSIZ]; // input string
    int dummy;             // dummy variable to check correct number of inputs

    do
    {
        fflush(stdin);
        printf("Enter a date as YYYY MM DD: ");
        fgets(inputStr, BUFSIZ, stdin);
        scanInputs = sscanf(inputStr, "%d %d %d %d", year, month, day, &dummy);
        if (scanInputs != 3)
        {
            printf("\tInvalid input! Enter exactly three inputs separated by spaces.");
            printf("\n\tExample input:\n\tEnter a date as YYYY MM DD: 2035 6 21\n");
        }
        else if (*year <= 0)
        {
            printf("\tInvalid input! The year must be positive.");
            printf("\n\tExample input:\n\tEnter a date as YYYY MM DD: 1969 7 16\n");
        }
        else if (*month < 1 || *month > 12)
        {
            printf("\tInvalid input! The month must be a month between 1 and 12.");
            printf("\n\tExample input:\n\tEnter a date as YYYY MM DD: 2021 2 18\n");
        }
        else if (*day < 1 || *day > monthLen(*month, *year))
        {
            printf("\tInvalid input! The day must be a valid day in the month you chose.");
            printf("\n\tExample input:\n\tEnter a date as YYYY MM DD: %04d %02d %02d\n", *year, *month, monthLen(*month, *year));
        }
    } while (scanInputs != 3 || *year <= 0 || *month < 1 || *month > 12 || *day < 1 || *day > monthLen(*month, *year));
}

// TIME MANIPULATION FUNCTIONS

/**
 * Determines whether a given year is a leap year
 *
 *  Inputs:
 * year: the year
 *
 *  Output:
 * 0 if the given year isn't a leapyear, 1 if it is.
 **/
int isLeapYear(int year)
{
    return (!(year % 4) && (year % 100)) || !(year % 400);
}

/**
 * Determines the number of days in a given month
 *
 *  Inputs:
 * month: the number of the month in the year
 * year: the year (to know whether february has an extra day)
 *
 *  Output:
 * The number of days in the month
 **/
int monthLen(int month, int year)
{
    int numDays; // number of days in the month

    if (month == FEB)
    {
        numDays = DAYSINMONTH + isLeapYear(year);
    }
    else if (month == APR || month == JUN || month == SEP || month == NOV)
    {
        numDays = DAYSMEDMONTH;
    }
    else
    {
        numDays = DAYSLONGMONTH;
    }

    return numDays;
}

/**
 * Counts the number of days that have occurred in the year at a given date (including the 1st day).
 *
 *  Inputs:
 * day: the day number in the month
 * month: the month number of the year
 * year: the year number
 *
 *  Output:
 * total number of days
 **/
int cumDaysInYr(int day, int month, int year)
{
    int days = 0; // total number of days in the year

    for (int i = 1; i < month; i += 1)
    {
        days += monthLen(i, year);
    }

    for (int i = 0; i < day; i++)
    {
        days += 1;
    }

    return days;
}

/**
 * Counts the number of leap years that have occurred since 2000 (works backward as well). Does not count the given year
 *
 *  Inputs:
 * year: the year to check up to
 *
 *  Output:
 * Number of years that have been leap years
 **/
int cumLpYrs(int year)
{
    int yrs = 0; // number of leap years since 2000 (works backward too)

    if (year > 2000)
    {
        for (int i = 2000; i < year; i++)
        {
            yrs += isLeapYear(i);
        }
    }
    else
    {
        for (int i = 2000; i > year; i--)
        {
            yrs -= isLeapYear(i);
        }
    }

    return yrs;
}

/**
 * Counts the total number of days between Jan 1 2000 and a given date (what you have to add to jan 1 to get the current date)
 *
 *  Inputs:
 * day: the day number in the month
 * month: the month number of the year
 * year: the year number
 *
 *  Output:
 * Total number of days
 **/
int cumDays(int day, int month, int year)
{
    int days = 0; // number of days since jan 1 2000 (should work backward)

    days += cumDaysInYr(day, month, year);
    days += DAYSINYEAR * (year - 2000);
    days += cumLpYrs(year);
    days -= (year >= 2000);

    return days;
}

/**
 * Calculates the Julain date for a given Gregorian date
 *
 *  Inputs:
 * day: the day number in the month
 * month: the month number of the year
 * year: the year number
 * timeZone: the time zone UTC offset
 *
 *  Output:
 * Julian date of the beginning of the day
 **/
double calcJDate(int day, int month, int year, double timeZone)
{
    double jDate; // Julian date for the given date

    jDate = JDATE2000;
    jDate += cumDays(day, month, year);

    return jDate;
}

/**
 * Reverse of the calcJDate function. Calculates the day/month/year for a given Julian date
 *
 *  Inputs:
 * jDate: the Julain date for the beginning of the day
 * tZ: the time zone in UTC offset
 * pointer day: variable in which to store the day number
 * pointer month: variable in which to store the month number
 * pointer year: variable in which to store the year number
 *
 *  Output:
 * None (pointers)
 **/
void calcDate(double jDate, double tZ, int *day, int *month, int *year)
{
    double currentJD; // current julian date (will default to the jDate of jan1 2000)

    *day = 1;
    *month = 1;
    *year = 2000;
    currentJD = calcJDate(*day, *month, *year, tZ);

    while (currentJD < jDate)
    {
        if (*day < monthLen(*month, *year))
        {
            *day += 1;
        }
        else if (*month < NUMMONTHS)
        {
            *month += 1;
            *day = 1;
        }
        else
        {
            *year += 1;
            *month = 1;
            *day = 1;
        }
        currentJD += 1;
    }

    while (currentJD > jDate)
    {
        if (*day > 1)
        {
            *day -= 1;
        }
        else if (*month > 1)
        {
            *month -= 1;
            *day = monthLen(*month, *year);
        }
        else
        {
            *year -= 1;
            *month = NUMMONTHS;
            *day = monthLen(*month, *year);
        }
        currentJD -= 1;
    }
}

/**
 * Rounds a given time to the nearest minute
 *
 *  Inputs:
 * timeDay: Decimal day. Range is [0:1)
 *
 *  Output:
 * Decimal day rounded to the nearest minute
 **/
double roundToMin(double timeDay)
{
    return round(timeDay * MININHR * HRSINDAY) / (MININHR * HRSINDAY);
}

/**
 * Finds the hour of a given time of day (in 24hr time)
 *
 *  Inputs:
 * timeDay: decimal day. Range is [0, 1)
 *
 *  Output:
 * Number of hours. Will be between 0 and 23 inclusive of both
 **/
int hours(double timeDay)
{
    return (int)(roundToMin(timeDay) * HRSINDAY) % HRSINDAY;
}

/**
 * Finds the hour of a given time of day, in 12hr time
 *
 *  Inputs:
 * timeDay: decimal day. Range is [0, 1)
 *
 *  Output:
 * Hour number on the clock, between 1 and 12 inclusive of both.
 **/
int hours12Hr(double timeDay)
{
    int hr; // hour on the clock. 1-12 inclusive

    hr = hours(timeDay) % HRS12HR;
    if (hr == 0)
    {
        hr = 12;
    }

    return hr;
}

/**
 * Determines if a given time is AM or PM
 *
 *  Inputs:
 * timeDay: decimal day. Range is [0, 1)
 *
 *  Output:
 * 'A' or 'P', depending on if it's AM or PM
 **/
char hrType12Hr(double timeDay)
{
    return (hours(timeDay) >= HRS12HR) ? 'P' : 'A';
}

/**
 * Finds the value of the minute hand at a given time
 *
 *  Inputs:
 * timeDay: decimal day. Range is [0, 1)
 *
 *  Output:
 * Number of minutes. Will be between 0 and 59 inclusive of both
 **/
int minutes(double timeDay)
{
    int mins; // minute on the clock. 0-59 inclusive

    mins = round(MININHR * HRSINDAY * roundToMin(timeDay));
    mins -= MININHR * hours(roundToMin(timeDay));

    return mins;
}

// SOLAR CALCULATION FUNCTIONS

/**
 * Calculates approximately when a given solar event happens
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day, locTime modifies for fractional days
 * tZ: time zone in UTC offset
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 * locTime: decimal day offset for when to check
 * event: The event to check:
 *  > 1: sunrise
 *  > 2: solar noon
 *  > 3: sunset
 *  > 0: day status
 *
 *  Output:
 * Decimal day time of event. -1 means it doesn't happen
 **/
double calcEventApprox(double jDate, double tZ, double longitude, double latitude, double locTime, int event)
{
    double geomMeanLongSun;  // Geometric mean longitude (L0) of the sun
    double geomMeanAnomSun;  // Geometric mean anomaly of the sun
    double eccentEarthOrbit; // Earth orbit eccentricity
    double sunEqCtr;         // Sun's equation of center (error of position with respect to a circular orbit of same period)
    double sunTruLong;       // Ecliptic Longitude of earth if the sun had 0 inclination
    double sunAppLong;       // Apparent Longitude of the sun
    double meanObliqEclip;   // Oblique ecliptic: Mean inclination of Earth’s equator relative to the Sun/planets apparent plane
    double obliqCorr;        // Oblique ecliptic corrected
    double sunDeclin;        // Sun's angular distance north/south of the equator
    double varY;             // ADD COMMENT
    double eqOfTime;         // Difference between apparent solar time and mean solar time
    double funcArg;          // argument of arccosine, so that we don't get an error if the value is of magnitude greater than 1
    double HASunrise;        // Cosine of the hour angle from the observer
    double jCent;            // Julian century

    double noon;           // solar noon (decimal day)
    double rise = locTime; // sunrise (decimal day)
    double set = locTime;  // sunset (decimal day)

    int status; // what the day does. 0 it starts and ends, 1 there's sunlight all 24hrs. -1 it's dark all 24hrs

    double eventTime; // Time of the given event.

    // locTime -= longitude / (15 * HRSINDAY);
    jDate += locTime;
    jCent = ((jDate - JDATE2000 - 1) / JULCENTURY);
    geomMeanLongSun = fmod(280.46646 + jCent * (36000.76983 + jCent * 0.0003032), 360);
    geomMeanAnomSun = 357.52911 + jCent * (35999.05029 - 0.0001537 * jCent);
    eccentEarthOrbit = 0.016708634 - jCent * (0.000042037 + 0.0000001267 * jCent);
    sunEqCtr = sind(geomMeanAnomSun) * (1.914602 - jCent * (0.004817 + 0.000014 * jCent)) + sind(2 * geomMeanAnomSun) * (0.019993 - 0.000101 * jCent) + sind(3 * geomMeanAnomSun) * 0.000289;
    sunTruLong = geomMeanLongSun + sunEqCtr;
    sunAppLong = sunTruLong - 0.00569 - 0.00478 * sind(125.04 - 1934.136 * jCent);
    meanObliqEclip = (HRSINDAY - 1) + (26 + ((21.448 - jCent * (46.815 + jCent * (0.00059 - jCent * 0.001813)))) / MININHR) / MININHR;
    obliqCorr = meanObliqEclip + 0.00256 * cosd(125.04 - 1934.136 * jCent);
    sunDeclin = asind(sind(obliqCorr) * sind(sunAppLong));
    varY = pow(tand(obliqCorr / 2), 2);
    eqOfTime = 4 * RAD2DEG * (varY * sind(2 * geomMeanLongSun) - 2 * eccentEarthOrbit * sind(geomMeanAnomSun) + 4 * eccentEarthOrbit * varY * sind(geomMeanAnomSun) * cosd(2 * geomMeanLongSun) - 0.5 * pow(varY, 2) * sind(4 * geomMeanLongSun) - 1.25 * pow(eccentEarthOrbit, 2) * sind(2 * geomMeanAnomSun));
    funcArg = sind(TWILIGHTANGLE) / (cosd(latitude) * cosd(sunDeclin)) - tand(latitude) * tand(sunDeclin);

    if (funcArg < -1)
    {
        status = 1;
        rise = -100;
        set = -100;
        noon = -100;
    }
    else if (funcArg > 1)
    {
        status = -1;
        rise = -100;
        set = -100;
        noon = -100;
    }
    else
    {
        status = 0;
        HASunrise = acosd(funcArg);
        noon = (720 - 4.0 * longitude - eqOfTime + tZ * MININHR) / (HRSINDAY * MININHR);
        rise = noon - HASunrise * 4.0 / (HRSINDAY * MININHR);
        set = noon + HASunrise * 4.0 / (HRSINDAY * MININHR);
    }

    switch (event)
    {
    case 1:
        eventTime = rise;
        break;
    case 2:
        eventTime = noon;
        break;
    case 3:
        eventTime = set;
        break;
    case 0:
        eventTime = status;
        break;
    }

    return eventTime;
}

/**
 * Uses the approximate function to predict exact sunrise/sunset
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day. Must be beginning of the day.
 * tZ: time zone in UTC offset
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 * event: The event to check:
 *  > 1: sunrise
 *  > 2: solar noon
 *  > 3: sunset
 *
 *  Output:
 * Decimal day time of event. -1 means it doesn't happen
 **/
double calcEvent(double jDate, double tZ, double longitude, double latitude, int event)
{
    double locTimePrev;    // local time from previous iteration
    double ansBegin;       // answer as of beginning of day
    double ansEnd;         // answer as of end of day
    double properTimeZone; // the time zone if it were perfect

    double finalAns; // final answer

    ansBegin = BEGINDAY;
    // printf("calcEvent called on jDate %.2f\n", jDate);

    properTimeZone = longitude / (15);

    do
    {
        locTimePrev = ansBegin;
        ansBegin = calcEventApprox(jDate, properTimeZone, longitude, latitude, ansBegin, event);
    } while (ansBegin >= -1 && ansBegin <= 2 && roundToMin(ansBegin) != roundToMin(locTimePrev));

    ansEnd = ENDDAY;
    do
    {
        locTimePrev = ansEnd;
        ansEnd = calcEventApprox(jDate, properTimeZone, longitude, latitude, ansEnd, event);
    } while (ansEnd >= -1 && ansEnd <= 2 && roundToMin(ansEnd) != roundToMin(locTimePrev));

    finalAns = fmax(ansEnd, ansBegin);

    finalAns -= properTimeZone / HRSINDAY;
    finalAns += tZ / HRSINDAY;

    return finalAns;
}

/**
 * Calculates what type of solar day it is / how many solar events happen
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day, locTime modifies for fractional days
 * tZ: time zone in UTC offset
 * longitude: East/west component of position
 * latitude: North/South component of position
 *
 *  Output:
 * -2: it's night for all 24hrs
 * -1: it's day for all 24hrs
 * 1: the sun rises OR sets, but not both
 * 2: the sun rises AND sets (normal day)
 **/
int calcDayType(double jDate, double tZ, double longitude, double latitude)
{
    int output = 0; // function output
    int statBegin;  // status as of beginning of day
    int statEnd;    // status as of end of day;

    statBegin = calcEventApprox(jDate, tZ, longitude, latitude, BEGINDAY, 0);
    statEnd = calcEventApprox(jDate, tZ, longitude, latitude, ENDDAY, 0);

    output += (!statBegin || !statEnd);
    output += (!statBegin && !statEnd);

    if (output == 0)
    {
        switch (statBegin)
        {
        case -1: // dark all 24hrs
            output = -2;
            break;
        case 1: // light all 24hrs
            output = -1;
            break;
        }
    }

    return output;
}

/**
 * Calculates the next or last specific solar event
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day
 * tZ: time zone in UTC offset
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 * locTime: decimal day offset for when to check
 * option: The event to find:
 *  > -1: last sunrise
 *  > 1: next sunrise
 *  > -3: last sunset
 *  > 3: next sunset
 *
 *  Output:
 * Julian date of the next day that the given event happens
 **/
double calcEventDay(double longitude, double latitude, double timeZone, double jDate, int option)
{
    int direction; // the sign of option
    int dayStatus; // what the day does
    int event;     // event, like whats used for the other functions

    event = abs(option);
    direction = (option > 0) - (option < 0);

    do
    {
        dayStatus = calcEvent(jDate, timeZone, longitude, latitude, event);
        jDate += direction;
    } while (dayStatus < -1 || dayStatus >= 2);

    jDate -= direction;

    return jDate;
}

// DISPLAY FUNCTIONS

/**
 * Prints coordinates with N/S and a degree symbol
 *
 *  Inputs:
 * longitude: longitude, with negative for west and positive for east
 * latitude: latitude, negative is south and positive is north
 *
 *  Output:
 * None
 **/
void printCoords(double longitude, double latitude)
{
    char latDirection = 'N';  // latitude cardinal direction
    char longDirection = 'E'; // longitude cardinal direction
    int longDecimals;         // number of decimal places in the longitude coordinate
    int latDecimals;          // number of decimal places in the latitude coordinate

    longDecimals = numDecimals(longitude);
    latDecimals = numDecimals(latitude);

    if (latitude < 0)
    {
        latDirection = 'S';
    }

    if (longitude < 0)
    {
        longDirection = 'W';
    }

    printf("%.*lf%c %c, %.*lf%c %c",
           latDecimals, fabs(latitude), DEGCODE, latDirection,
           longDecimals, fabs(longitude), DEGCODE, longDirection);
}

/**
 * Prints a date to as DD Monthname, YYYY
 *
 *  Inputs:
 * jDate: Julian date to print
 *
 *  Output:
 * None
 **/
void printDate(double jDate)
{
    int day;   // day of the month
    int month; // month of the year
    int year;  // standard calendar year

    calcDate(jDate, 0, &day, &month, &year);

    printf("%02d ", day);
    switch (month)
    {
    case JAN:
        printf("January");
        break;
    case FEB:
        printf("February");
        break;
    case MAR:
        printf("March");
        break;
    case APR:
        printf("April");
        break;
    case MAY:
        printf("May");
        break;
    case JUN:
        printf("June");
        break;
    case JUL:
        printf("July");
        break;
    case AUG:
        printf("August");
        break;
    case SEP:
        printf("September");
        break;
    case OCT:
        printf("October");
        break;
    case NOV:
        printf("November");
        break;
    case DEC:
        printf("December");
        break;
    }
    printf(", %d", year);
}

/**
 * Prints a time in both 24 and 12 hour format
 *
 *  Inputs:
 * fracDay: the time of day as a decimal day
 *
 *  Output:
 * None
 **/
void dispTime(double fracDay)
{
    int hrs;   // hours
    int min;   // mins
    int hrs12; // 12hr hours

    hrs = hours(fracDay);
    hrs12 = hours12Hr(fracDay);
    min = minutes(fracDay);

    if (fracDay >= 0 && fracDay < 1)
    {
        printf("%02d:%02d (%d:%02d %cM)", hours(fracDay), minutes(fracDay),
               hours12Hr(fracDay), minutes(fracDay), hrType12Hr(fracDay));
    }
    else if (fracDay >= -1 && fracDay < 0)
    {
        fracDay++;
        printf("%02d:%02d (%d:%02d %cM) the previous date", hours(fracDay), minutes(fracDay),
               hours12Hr(fracDay), minutes(fracDay), hrType12Hr(fracDay));
    }
    else if (fracDay >= 1 && fracDay < 2)
    {
        fracDay--;
        printf("%02d:%02d (%d:%02d %cM) the previous date", hours(fracDay), minutes(fracDay),
               hours12Hr(fracDay), minutes(fracDay), hrType12Hr(fracDay));
    }
    else
    {
        printf("None");
    }
}

/**
 * Prints the output for a nominal day with a sunrise or sunset. Shows sunrise, sunset, and solar noon times
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day
 * tZ: time zone in UTC offset
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 *
 *  Output:
 * None
 **/
void nominalOutput(double jDate, double timeZone, double longitude, double latitude)
{
    double riseTime; // sunrise time (decimal day - local time)
    double setTime;  // sunset time (decimal day - local time)
    double solNoon;  // solar noon (decimal day - local time)
    double duration; // amount of sunlight (decimal day)

    riseTime = calcEvent(jDate, timeZone, longitude, latitude, 1);
    solNoon = calcEvent(jDate, timeZone, longitude, latitude, 2);
    setTime = calcEvent(jDate, timeZone, longitude, latitude, 3);
    duration = setTime - riseTime;

    printf("\nSunrise: ");
    dispTime(riseTime);

    printf("\nSolar Noon: ");
    dispTime(solNoon);

    printf("\nSunset: ");
    dispTime(setTime);

    if (setTime >= -1 && riseTime >= -1)
    {
        printf("\nThere are %d hrs and %d minutes of sunlight", hours(duration), minutes(duration));
    }

    printf("\n\n");
}

/**
 * Prints the output for a day with neither sunrise nor sunset. Displays last event and next event dates and times.
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day
 * tZ: time zone in UTC offset
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 * status: The day type as determined by calcDayType
 *
 *  Output:
 * None
 **/
void extremeLatOutput(double jDate, double timeZone, double longitude, double latitude, int status)
{
    double lastEventDay;  // Julian date for the last event
    double nextEventDay;  // Julain date for the next event
    double lastEventTime; // time for the last event (decimal day)
    double nextEventTime; // time for the next event (decimal day)
    double noon;          // solar noon time
    int numDays;          // number of days between two events
    int numHrs;           // number of excess hours between two events
    int numMins;          // number of excess minutes between two events
    double diff;          // difference between jDates of the two events (decimal day)

    switch (status)
    {
    case -1:
        lastEventDay = calcEventDay(longitude, latitude, timeZone, jDate, -1);
        nextEventDay = calcEventDay(longitude, latitude, timeZone, jDate, 3);
        lastEventTime = calcEvent(lastEventDay, timeZone, longitude, latitude, 1);
        nextEventTime = calcEvent(nextEventDay, timeZone, longitude, latitude, 3);
        break;
    case -2:
        lastEventDay = calcEventDay(longitude, latitude, timeZone, jDate, -3);
        nextEventDay = calcEventDay(longitude, latitude, timeZone, jDate, 1);
        nextEventTime = calcEvent(nextEventDay, timeZone, longitude, latitude, 1);
        lastEventTime = calcEvent(lastEventDay, timeZone, longitude, latitude, 3);
    }
    noon = calcEvent(jDate, timeZone, longitude, latitude, 2);

    numMins = (minutes(1 - lastEventTime) + minutes(nextEventTime)) % MININHR;
    numHrs = hours(1 - lastEventTime) + hours(nextEventTime) + (minutes(1 - lastEventTime) + minutes(nextEventTime)) / MININHR;
    diff = nextEventDay - lastEventDay;
    numDays = floor(diff - (numHrs / (double)HRSINDAY) - numMins / ((double)HRSINDAY * MININHR));

    switch (status)
    {
    case -1:
        printf("The sun doesn't set. It's day for all 24hrs.\n\n");
        printf("\nThe sun rose on ");
        printDate(lastEventDay);
        printf(", at ");
        dispTime(lastEventTime);
        printf("\nThe sun will set on ");
        printDate(nextEventDay);
        printf(", at ");
        dispTime(nextEventTime);
        printf("\nThe solar day lasts %d days, %d hours, and %d minutes", numDays, numHrs, numMins);
        break;
    case -2:
        printf("The sun doesn't rise. It's night for all 24hrs.\n\n");
        printf("The sun set on ");
        printDate(lastEventDay);
        printf(", at ");
        dispTime(lastEventTime);
        printf("\nThe sun will rise on ");
        printDate(nextEventDay);
        printf(", at ");
        dispTime(nextEventTime);
        printf("\nThe solar night lasts %d days, %d hours, and %d minutes", numDays, numHrs, numMins);
        break;
    }
}

/**
 * Calls the respective print functions to print the total output
 *
 *  Inputs:
 * jDate: Julian date to check. Must be the beginning of a day
 * longitude: East/west component of position find solar event
 * latitude: North/South component of position find solar event
 *
 *  Output:
 * None
 **/
void printAll(double jDate, double timeZone, double longitude, double latitude)
{
    int dayStatus; // whether the sun rises, sets, or both
    int dayType;   // The type of day: all night, all day, sunrise and sunsent, or just one?

    dayType = calcDayType(jDate, timeZone, longitude, latitude);

    printf("\n\n\t");
    printDate(jDate);
    printf("\n\t");
    printCoords(longitude, latitude);
    printf("\n\n");
    if (dayType > 0)
    { // there's a sunrise or sunset
        nominalOutput(jDate, timeZone, longitude, latitude);
    }
    else
    { // there's neither a sunrise nor a sunset
        extremeLatOutput(jDate, timeZone, longitude, latitude, dayType);
    }
}