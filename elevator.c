// REPLACE THIS FILE WITH YOUR OWN CODE.
// READ ELEV.H FOR INFORMATION ON HOW TO USE THE ELEVATOR FUNCTIONS.

#include "elev.h"
#include "elevator.h"
#include "orderqueue.h"
#include "timer.h"
#include "panel.h"
#include <stdio.h>
#include <unistd.h>

/**
 Defining static variables, only invisible in this file
 **/
static elevatorState currentState, nextState;

static int directionUp;
static int currentFloor;
/**
 All the signals used in the statemachine
 **/
static int signalHasOrders;
static int signalShouldStop;
static int signalEmergencyStop;
static int signalTimerIsFinished;
static int signalObstruction;

int initElevator() {
    if (!elev_init()) {
        printf(__FILE__ ": Unable to initialize elevator hardware\n");
        return 0;
    }
    if (!initQueue())
        return 0;
  directionUp = 1;
  elev_set_speed(100);
  while (elev_get_floor_sensor_signal()== -1) {
    ;
  }
  stopElevator();
  currentState = IDLE;
  nextState = IDLE;
  currentFloor = elev_get_floor_sensor_signal();
  signalHasOrders = 0;
  signalShouldStop = 0;
  signalEmergencyStop = 0;
  signalTimerIsFinished = 0;
  signalObstruction = 0;
    return 1;
}

int main()
{
    // Initialize hardware
    printf("Initializing elevator");


    initElevator();
    
    printf("Press STOP button to stop elevator and exit program.\n");
      
    while (1) {
      switch (currentState) {
      case IDLE:
	if (signalHasOrders) nextState = DRIVE;
	else nextState = IDLE;
        break;

      case DRIVE:
	if (signalShouldStop) nextState = OPENDOOR;
	else nextState = DRIVE;
	break;
      case OPENDOOR:
	if (signalTimerIsFinished) nextState = CLOSEDOOR;
	else nextState = OPENDOOR;
	break;
      case CLOSEDOOR:
	if (signalObstruction) nextState = OPENDOOR;
	else if (signalHasOrders) nextState = DRIVE;
	else nextState = IDLE;
	break;
      case EMERGENCYSTOP:
	if (signalHasOrders) nextState = DRIVE;
	else nextState = EMERGENCYSTOP;
	break;
      }

      if (signalEmergencyStop) nextState = EMERGENCYSTOP;

      if (nextState != currentState) {
	switch (nextState) {
	case IDLE:
	  break;
	case DRIVE:
      directionUp = findDirection();
	  setSpeed();
	  break;
	case OPENDOOR:
	  stopElevator();
	  deleteOrderInFloor(currentFloor);
	  turnOffLightsInFloor(currentFloor);
	  startTimer();
	  break;
	case CLOSEDOOR:
	  break;
	case EMERGENCYSTOP:
	  stopElevator();
	  deleteAllOrders();
	  break;
	}
      }

      currentState = nextState;
      updateSignals(currentState);
      updatePanel();
      printStatus();
        // Check if stop button, if so, stop elevator and exit program.
      if (elev_get_stop_signal()) {
	elev_set_speed(0);
	break;
      }
    }
    return 0;
}

 void updateSignals(elevatorState curState) {
   switch(curState) {
   case IDLE:
     signalHasOrders = hasOrders();
     break;
   case DRIVE:
     if (elev_get_floor_sensor_signal() != -1) {
       currentFloor = elev_get_floor_sensor_signal();
       if (hasOrderInFloor(directionUp, currentFloor) || (findDirection() == !directionUp)) signalShouldStop = 1;
     }
     else signalShouldStop = 0;
     break;
   case OPENDOOR:
     signalTimerIsFinished = timerIsFinished(3);
     break;
   case CLOSEDOOR:
     signalHasOrders = hasOrders();
     signalObstruction = elev_get_obstruction_signal();
     break;
   case EMERGENCYSTOP:
     break;
   }
   signalEmergencyStop=elev_get_stop_signal();
 }

void setSpeed(void) {
   if (directionUp == UP)
     elev_set_speed(300);
   else elev_set_speed(-300);
 }

int findDirection() {
  int floor = currentFloor;
  if (directionUp == UP) {
    for (floor = floor+1; floor<N_FLOORS; floor++) {
      if (hasOrderInFloor(UP, floor) || hasOrderInFloor(DOWN, floor))
          return UP;
    }
    return DOWN;
  }
  else if (directionUp == DOWN) {
    for (floor = floor-1; floor>=0; floor--) {
      if (hasOrderInFloor(UP, floor) || hasOrderInFloor(DOWN, floor))
	    return DOWN;
    }
    return UP;
  }
  printf("orderqueue error: direction neither up nor down");
  return UP;
}

void stopElevator(){
  if(directionUp==UP) elev_set_speed(-100);
  else elev_set_speed(100);
  usleep(100000);
  elev_set_speed(0);
    signalShouldStop = 0;
}

int getCurrentFloor() {
  return currentFloor;
}

void printStatus () {
  printf("Current Floor: %i, HasOrders: %i, directionUP: %i, signalShouldStop = %i, currentState = %i, nextState = %i\n", currentFloor, hasOrders(), directionUp, signalShouldStop, currentState, nextState);
}





