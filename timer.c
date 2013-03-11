#include "timer.h"
#include <stdlib.h>


static time_t theTime;

void timer_start(void) {
    theTime = time(NULL);
}

int timer_timerIsFinished(double sek) {
    static time_t timeNow;
    timeNow = time(NULL);
    if (difftime(timeNow, theTime) >= sek)
        return 1;
    return 0;
}
