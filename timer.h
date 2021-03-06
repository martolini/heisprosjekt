#pragma once

#include <time.h>
#include <stdio.h>

/**
 Starting the timer.
 **/
void timer_start(void);

/**
 Checking if the timer is finished.
 @param seconds Number of seconds the timer should count to.
 @return 1 when timer is finished, 0 otherwise.
 **/
int timer_timerIsFinished(double);
