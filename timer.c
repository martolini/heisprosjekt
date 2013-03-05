#include "timer.h"
#include <stdlib.h>


time_t theTime;

void startTimer(void) {
    theTime = time(NULL);
    printf("timer started\n");
}

int timerIsFinished(double sek) {
    static time_t timeNow;
    timeNow = time(NULL);
    if (difftime(timeNow, theTime) >= sek)
        return 1;
    return 0;
}
