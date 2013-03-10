#include "timer.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#import <sys/time.h>


static time_t theTime;
static timeval *timeStart;

void timer_start(void) {
    theTime = time(NULL);
    gettimeofday(timeStart);
    printf("timer started\n");
}

int timer_timerIsFinished(double sek) {
    static timeval *timeNow;
    gettimeofday(timeNow);
    double diff = timeNow.tv_usec-timeStart.tv_usec;
    if (diff >= 3)
        return 1;
    return 0;
//    static time_t timeNow;
//    timeNow = time(NULL);
//    printf("%f", difftime(timeNow, theTime));
//    if (difftime(timeNow, theTime) >= sek)
//        return 1;
//    return 0;
}
