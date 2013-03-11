#pragma once

#include "globals.h"

/**
 Sets the speed of the elevator.
 @param speed New speed of elevator. Positive values denote upward movement
 and vice versa. Set speed to 0 to stop the elevator. From -300 to 300 gives
 sensible speeds.
 */
void elev_setSpeed(int speed);

/**
 Updating all the signals for the statemachine
 @param param The parameters of the elevator
 **/
void elev_updateSignals(elevatorParameters_t *);


/**
 Initializing the elevator
 @param param the parameters of the elevator
 @return Nonzero on success, 0 on failure.
 **/
int elev_init(elevatorParameters_t *);

/**
 Calculating the next direction of the elevator, depending on the orderqueue.
 @param1 currentFloor The current floor of the elevator
 @param2 direction The current direction of the elevator
 @return 1 for UP and 0 for DOWN (enum).
 **/
elevatorDirection_t elev_findDirection(int, int);

/**
 Stopping the elevator.
 @param direction The direction of the elevator
 **/
void elev_stop(elevatorDirection_t);

/**
 Setting the direction and speed of the elevator
 @param param The parameters of the elevator
 **/
void elev_setDirectionAndSpeed(elevatorParameters_t *);

/**
 Prints the status of the elevator with states and signals. Mainly for debugging.
 **/
void printStatus(void);
