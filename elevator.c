// REPLACE THIS FILE WITH YOUR OWN CODE.
// READ ELEV.H FOR INFORMATION ON HOW TO USE THE ELEVATOR FUNCTIONS.

#include "elevator.h"
#include "orderqueue.h"
#include "timer.h"
#include "panel.h"
#include "io.h"
#include "channels.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


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

int elev_init(void) {
    int i;
    // Init hardware
    if (!io_init())
        return 0;
    
    // Zero all floor button lamps
    for (i = 0; i < N_FLOORS; ++i) {
        if (i != 0)
            panel_setButtonLamp(BUTTON_CALL_DOWN, i, 0);
        
        if (i != N_FLOORS-1)
            panel_setButtonLamp(BUTTON_CALL_UP, i, 0);
        
        panel_setButtonLamp(BUTTON_COMMAND, i, 0);
    }
    
    // Clear stop lamp, door open lamp, and set floor indicator to ground floor.
    panel_setStopLamp(0);
    panel_setDoorOpenLamp(0);
    panel_setFloorIndicator(0);
    if (!oq_init())
        return 0;
    directionUp = 1;
    elev_setSpeed(100);
    while (panel_getFloorSensorSignal()== -1) {
        ;
    }
    elev_stop();
    currentState = IDLE;
    nextState = IDLE;
    currentFloor = panel_getFloorSensorSignal();
    panel_setFloorIndicator(currentFloor);
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
    
    
    elev_init();
    
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
                    elev_setDirectionAndSpeed();
                    break;
                case OPENDOOR:
                    if (currentState == DRIVE) elev_stop();
                    oq_deleteOrderInFloor(currentFloor);
                    panel_turnOffLightsInFloor(currentFloor);
                    panel_setDoorOpenLamp(LAMP_ON);
                    timer_start();
                    break;
                case CLOSEDOOR:
                    if (currentState == EMERGENCYSTOP) panel_setStopLamp(LAMP_OFF);
                    panel_setDoorOpenLamp(LAMP_OFF);
                    break;
                case EMERGENCYSTOP:
                    if (currentState == DRIVE)
                        elev_stop();
                    oq_deleteAllOrders();
                    panel_turnOffAllLights();
                    panel_setStopLamp(LAMP_ON);
                    break;
            }
        }
        
        currentState = nextState;
        elev_updateSignals(currentState);
        panel_checkForOrders();
        printStatus();
    }
    return 0;
}

void elev_updateSignals(elevatorState curState) {
    switch(curState) {
        case IDLE:
            signalHasOrders = oq_hasOrders();
            signalShouldStop = (oq_hasOrderInFloor(UP, currentFloor) || oq_hasOrderInFloor(DOWN, currentFloor));
            break;
        case DRIVE:;
            int tempFloor = panel_getFloorSensorSignal(); // Had to do it, sometimes currentFloor got -1 ?!?!!??!!?
            if (tempFloor != -1) {
                currentFloor = tempFloor;
                panel_setFloorIndicator(currentFloor);
                if (oq_hasOrderInFloor(directionUp, currentFloor) || (elev_findDirection() == !directionUp)) signalShouldStop = 1;
                else signalShouldStop = 0;
            }
            else signalShouldStop = 0;
            break;
        case OPENDOOR:
            signalTimerIsFinished = timer_timerIsFinished(3);
            break;
        case CLOSEDOOR:
            signalHasOrders = oq_hasOrders();
            signalObstruction = panel_getObstructionSignal();
            break;
        case EMERGENCYSTOP:;
            int tempFloor = panel_getFloorSensorSignal();
            if (tempFloor != -1) {
                currentFloor = tempFloor;
                panel_setFloorIndicator(currentFloor);
            }
            signalHasOrders = oq_hasOrders();
            
            break;
    }
    signalEmergencyStop=panel_getStopSignal();
}

elevatorDirection elev_findDirection(void) {
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

void elev_stop(void){
    if(directionUp==UP) elev_setSpeed(-100);
    else elev_setSpeed(100);
    usleep(50000);
    elev_setSpeed(0);
}

int elev_getCurrentFloor(void) {
    return currentFloor;
}

void printStatus (void) {
    printf("Current Floor: %i, HasOrders: %i, directionUP: %i, signalShouldStop = %i, currentState = %i, nextState = %i\n", currentFloor, oq_hasOrders(), directionUp, signalShouldStop, currentState, nextState);
}

elevatorState elev_getCurrentElevatorState(void) {
    return currentState;
}

void elev_setDirectionAndSpeed() {
    directionUp = elev_findDirection();
    if (directionUp == UP)
        elev_setSpeed(300);
    else elev_setSpeed(-300);
}

void elev_setSpeed(int speed)
{
    // In order to sharply stop the elevator, the direction bit is toggled
    // before setting speed to zero.
    static int last_speed = 0;
    
    // If to start (speed > 0)
    if (speed > 0)
        io_clear_bit(MOTORDIR);
    else if (speed < 0)
        io_set_bit(MOTORDIR);
    
    // If to stop (speed == 0)
    else if (last_speed < 0)
        io_clear_bit(MOTORDIR);
    else if (last_speed > 0)
        io_set_bit(MOTORDIR);
    
    last_speed = speed ;
    
    // Write new setting to motor.
    io_write_analog(MOTOR, 2048 + 4*abs(speed));
}







