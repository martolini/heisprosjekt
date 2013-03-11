#include "timer.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>


static time_t theTime;
static struct timeval start;

void timer_start(void) {
    theTime = time(NULL);
    gettimeofday(&start, NULL);
}

int timer_timerIsFinished(double sek) {
    static time_t timeNow;
    static struct timeval end;
    gettimeofday(&end, NULL);
    double secs = (end.tv_sec - start.tv_sec) * 1000
    + (end.tv_usec - start.tv_usec) / 1000;
    printf("%f\n", secs);
    if (secs/1000 >= sek)
        return 1;
    return 0;
    
    timeNow = time(NULL);
    if (difftime(timeNow, theTime) >= sek)
        return 1;
    return 0;
}
