#pragma once

/**
 Defines all the states of the statemachine
 **/
typedef enum {
  IDLE = 0,
  DRIVE,
  OPENDOOR,
  CLOSEDOOR,
  EMERGENCYSTOP
} elevatorState;


/**
 Updating all the signals for the statemachine, dependant on the state
 **/
void updateSignals(elevatorState curState);

/**
 Setting the speed of the elevator, checking for direction
 **/
void setSpeed(void);

/**
 Initializing the elevator
 @return Nonzero on success, 0 on failure.
 **/
int initElevator(void);

/**
 Calculating the next direction of the elevator, depending on the orderqueue.
 @return 1 for UP and 0 for DOWN (enum).
 **/
int findDirection();

/**
 Stopping the elevator.
 **/
void stopElevator(void);

/**
 Returning the current floor
 @return the current floor, 0-indexed.
 **/
int getCurrentFloor(void);

/**
 Prints the status of the elevator with states and signals. Mainly for debugging.
 **/
void printStatus();
