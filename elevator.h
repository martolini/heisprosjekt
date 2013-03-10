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
 Updating all the signals for the statemachine, dependant on the state
 **/
void elev_updateSignals(elevatorState curState);


/**
 Initializing the elevator
 @return Nonzero on success, 0 on failure.
 **/
int elev_init(void);

/**
 Calculating the next direction of the elevator, depending on the orderqueue.
 @return 1 for UP and 0 for DOWN (enum).
 **/
elevatorDirection elev_findDirection(void);

/**
 Stopping the elevator.
 **/
void elev_stop(void);

/**
 Returning the current floor
 @return the current floor, 0-indexed.
 **/
int elev_getCurrentFloor(void);

/**
 Returns the current state of the elevator
 **/
elevatorState elev_getCurrentElevatorState(void);

/**
 Setting the direction and speed of the elevator
 **/
void elev_setDirectionAndSpeed(void);

/**
 Prints the status of the elevator with states and signals. Mainly for debugging.
 **/
void printStatus(void);
