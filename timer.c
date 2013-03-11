#include "timer.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


static struct timeval startTime;

void timer_start(void) {
    theTime = time(NULL);
    gettimeofday(&startTime, NULL);
}

int timer_timerIsFinished(double sek) {
    static struct timeval endTime;
    gettimeofday(&end, NULL);
    double secs = (endTime.tv_sec - startTime.tv_sec) * 1000
    + (endTime.tv_usec - startTime.tv_usec) / 1000;
    if (secs/1000 >= sek)
        return 1;
    return 0;
}
