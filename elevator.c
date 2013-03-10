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

static elevatorDirection directionUp;
static int currentFloor;
/**
 All the signals used in the statemachine
 **/

static int signalHasOrders;
static int signalShouldStop;
static int signalEmergencyStop;
static int signalTimerIsFinished;
static int signalObstruction;

int initElevator(void) {
    if (!elev_init()) {
        printf(__FILE__ ": Unable to initialize elevator hardware\n");
        return 0;
    }
    if (!oq_init())
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
    elev_set_floor_indicator(currentFloor);
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
                if (signalHasOrders) {
                    if (signalShouldStop)
                        nextState = OPENDOOR;
                    else nextState = DRIVE;
                }
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
                if (signalHasOrders) nextState = CLOSEDOOR; // Making sure doors are closed and everything's in order before continuing.
                else nextState = EMERGENCYSTOP;
                break;
        }
        
        if (signalEmergencyStop) nextState = EMERGENCYSTOP;
        
        if (nextState != currentState) {
            switch (nextState) {
                case IDLE:
                    break;
                case DRIVE:
                    setDirectionAndSpeed();
                    break;
                case OPENDOOR:
                    if (currentState == DRIVE) stopElevator();
                    oq_deleteOrderInFloor(currentFloor);
                    panel_turnOffLightsInFloor(currentFloor);
                    elev_set_door_open_lamp(LAMP_ON);
                    timer_start();
                    break;
                case CLOSEDOOR:
                    if (currentState == EMERGENCYSTOP) elev_set_stop_lamp(LAMP_OFF);
                    elev_set_door_open_lamp(LAMP_OFF);
                    break;
                case EMERGENCYSTOP:
                    if (currentState == DRIVE)
                        stopElevator();
                    oq_deleteAllOrders();
                    panel_turnOffAllLights();
                    elev_set_stop_lamp(LAMP_ON);
                    break;
            }
        }
        
        currentState = nextState;
        updateSignals(currentState);
        panel_checkForOrders();
        printStatus();
    }
    return 0;
}

void updateSignals(elevatorState curState) {
    switch(curState) {
        case IDLE:
            signalHasOrders = oq_hasOrders();
            signalShouldStop = (oq_hasOrderInFloor(UP, currentFloor) || oq_hasOrderInFloor(DOWN, currentFloor));
            break;
        case DRIVE:;
            int tempFloor = elev_get_floor_sensor_signal(); // Had to do it, sometimes currentFloor got -1 ?!?!!??!!?
            if (tempFloor != -1) {
                currentFloor = tempFloor;
                elev_set_floor_indicator(currentFloor);
                if (oq_hasOrderInFloor(directionUp, currentFloor) || (findDirection() == !directionUp)) signalShouldStop = 1;
                else signalShouldStop = 0;
            }
            else signalShouldStop = 0;
            break;
        case OPENDOOR:
            signalTimerIsFinished = timer_timerIsFinished(3);
            break;
        case CLOSEDOOR:
            signalHasOrders = oq_hasOrders();
            signalObstruction = elev_get_obstruction_signal();
            break;
        case EMERGENCYSTOP:
            signalHasOrders = oq_hasOrders();
            break;
    }
    signalEmergencyStop=elev_get_stop_signal();
}

elevatorDirection findDirection(void) {
    int floor = currentFloor;
    if (directionUp == UP) {
        for (floor = floor+1; floor<N_FLOORS; floor++) {
            if (oq_hasOrderInFloor(UP, floor) || oq_hasOrderInFloor(DOWN, floor))
                return UP;
        }
        return DOWN;
    }
    else if (directionUp == DOWN) {
        for (floor = floor-1; floor>=0; floor--) {
            if (oq_hasOrderInFloor(UP, floor) || oq_hasOrderInFloor(DOWN, floor))
                return DOWN;
        }
        return UP;
    }
    printf("orderqueue error: direction neither up nor down");
    return UP;
}

void stopElevator(void){
    if(directionUp==UP) elev_set_speed(-100);
    else elev_set_speed(100);
    usleep(50000);
    elev_set_speed(0);
}

int getCurrentFloor(void) {
    return currentFloor;
}

void printStatus (void) {
    printf("Current Floor: %i, HasOrders: %i, directionUP: %i, signalShouldStop = %i, currentState = %i, nextState = %i\n", currentFloor, oq_hasOrders(), directionUp, signalShouldStop, currentState, nextState);
}

elevatorState getCurrentElevatorState(void) {
    return currentState;
}

void setDirectionAndSpeed() {
    directionUp = findDirection();
    if (directionUp == UP)
        elev_set_speed(300);
    else elev_set_speed(-300);
}







