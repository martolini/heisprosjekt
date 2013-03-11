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

int elev_run(void) {
    printf("Initializing elevator");
    
    elevatorParameters_t l_elevParam;
    elevatorParameters_t *elevParam = &l_elevParam;
    elev_init(elevParam);
        
    while (1) {
        switch (elevParam->currentState) {
            case IDLE:
                if (elevParam->signals[HAS_ORDERS]) {
                    if (elevParam->signals[SHOULD_STOP])
                        elevParam->nextState = OPENDOOR;
                    else elevParam->nextState = DRIVE;
                }
                else elevParam->nextState = IDLE;
                break;
                
            case DRIVE:
                if (elevParam->signals[SHOULD_STOP]) elevParam->nextState = OPENDOOR;
                else elevParam->nextState = DRIVE;
                break;
            case OPENDOOR:
                if (elevParam->signals[TIMER_FINISHED]) elevParam->nextState = CLOSEDOOR;
                else elevParam->nextState = OPENDOOR;
                break;
            case CLOSEDOOR:
                if (elevParam->signals[OBSTRUCTION]) elevParam->nextState = OPENDOOR;
                else if (elevParam->signals[HAS_ORDERS]) elevParam->nextState = DRIVE;
                else elevParam->nextState = IDLE;
                break;
            case EMERGENCYSTOP:
                if (elevParam->signals[HAS_ORDERS]) elevParam->nextState = CLOSEDOOR; // Making sure doors are closed and everything's in order before continuing.
                else elevParam->nextState = EMERGENCYSTOP;
                break;
        }
        
        if (elevParam->signals[EMERGENCY]) elevParam->nextState = EMERGENCYSTOP;
        
        if (elevParam->nextState != elevParam->currentState) {
            switch (elevParam->nextState) {
                case IDLE:
                    break;
                case DRIVE:
                    elev_setDirectionAndSpeed(elevParam);
                    break;
                case OPENDOOR:
                    if (elevParam->currentState == DRIVE) elev_stop(elevParam->directionUp);
                    oq_deleteOrderInFloor(elevParam->currentFloor);
                    panel_turnOffLightsInFloor(elevParam->currentFloor);
                    panel_setDoorOpenLamp(LAMP_ON);
                    timer_start();
                    break;
                case CLOSEDOOR:
                    if (elevParam->currentState == EMERGENCYSTOP) panel_setStopLamp(LAMP_OFF);
                    panel_setDoorOpenLamp(LAMP_OFF);
                    break;
                case EMERGENCYSTOP:
                    if (elevParam->currentState == DRIVE)
                        elev_stop(elevParam->directionUp);
                    oq_deleteAllOrders();
                    panel_turnOffAllLights();
                    panel_setStopLamp(LAMP_ON);
                    break;
            }
        }
        
        elevParam->currentState = elevParam->nextState;
        elev_updateSignals(elevParam);
        panel_checkForOrders(elevParam);
        printStatus();
    }
}

int elev_init(elevatorParameters_t *param) {
    int i;
    // Init hardware
    if (!io_init())
        return 0;
    
    // Zero all floor button lamps
    for (i = 0; i < N_FLOORS; ++i) {
        if (i != 0)
            panel_setButtonLamp(BUTTON_CALL_DOWN, i, LAMP_OFF);
        
        if (i != N_FLOORS-1)
            panel_setButtonLamp(BUTTON_CALL_UP, i, LAMP_OFF);
        
        panel_setButtonLamp(BUTTON_COMMAND, i, LAMP_OFF);
    }
    
    // Clear stop lamp, door open lamp, and set floor indicator to ground floor.
    panel_setStopLamp(LAMP_OFF);
    panel_setDoorOpenLamp(LAMP_OFF);
    panel_setFloorIndicator(LAMP_ON);
    if (!oq_init())
        return 0;
    elev_setSpeed(100);
    while (panel_getFloorSensorSignal()== -1) {
        ;
    }
    elev_stop(UP);
    param->currentState = IDLE;
    param->nextState = IDLE;
    for (i=0; i<NUMBER_OF_SIGNALS; ++i)
        param->signals[i] = 0;
    param->directionUp = UP;
    param->currentFloor = panel_getFloorSensorSignal();
    panel_setFloorIndicator(param->currentFloor);
    return 1;
}

void elev_updateSignals(elevatorParameters_t *param) {
    switch(param->currentState) {
        case IDLE:
            param->signals[HAS_ORDERS] = oq_hasOrders();
            param->signals[SHOULD_STOP] = (oq_hasOrderInFloor(UP, param->currentFloor) || oq_hasOrderInFloor(DOWN, param->currentFloor));
            break;
        case DRIVE:;
            int tempFloor = panel_getFloorSensorSignal(); // Had to do it, sometimes currentFloor got -1 ?!?!!??!!?
            if (tempFloor != -1) {
                param->currentFloor = tempFloor;
                panel_setFloorIndicator(param->currentFloor);
                if (oq_hasOrderInFloor(param->directionUp, param->currentFloor) || (elev_findDirection(param->currentFloor, param->directionUp) == !param->directionUp))
                    param->signals[SHOULD_STOP] = 1;
                else param->signals[SHOULD_STOP] = 0;
            }
            else param->signals[SHOULD_STOP] = 0;
            break;
        case OPENDOOR:
            param->signals[TIMER_FINISHED] = timer_timerIsFinished(3);
            break;
        case CLOSEDOOR:
            param->signals[HAS_ORDERS] = oq_hasOrders();
            param->signals[OBSTRUCTION] = panel_getObstructionSignal();
            break;
        case EMERGENCYSTOP:;
            tempFloor = panel_getFloorSensorSignal();
            if (tempFloor != -1) {
                param->currentFloor = tempFloor;
                panel_setFloorIndicator(param->currentFloor);
            }
            param->signals[HAS_ORDERS] = oq_hasOrders();
            break;
    }
    param->signals[EMERGENCY] = panel_getStopSignal();
}

elevatorDirection_t elev_findDirection(int currentFloor, int direction) {
    int floor = currentFloor;
    if (direction == UP) {
        for (floor = floor+1; floor<N_FLOORS; floor++) {
            if (oq_hasOrderInFloor(UP, floor) || oq_hasOrderInFloor(DOWN, floor))
                return UP;
        }
        return DOWN;
    }
    else if (direction == DOWN) {
        for (floor = floor-1; floor>=0; floor--) {
            if (oq_hasOrderInFloor(UP, floor) || oq_hasOrderInFloor(DOWN, floor))
                return DOWN;
        }
        return UP;
    }
    printf("orderqueue error: direction neither up nor down");
    return UP;
}

void elev_stop(elevatorDirection_t direction){
    if (direction == UP) elev_setSpeed(-100);
    else elev_setSpeed(100);
    usleep(50000);
    elev_setSpeed(0);
}

void printStatus (void) {
    //printf("Current Floor: %i, HasOrders: %i, directionUP: %i, signalShouldStop = %i, currentState = %i, elevParam->nextState = %i\n", currentFloor, oq_hasOrders(), directionUp, signalShouldStop, currentState, elevParam->nextState);
}


void elev_setDirectionAndSpeed(elevatorParameters_t *param) {
    param->directionUp = elev_findDirection(param->currentFloor, param->directionUp);
    if (param->directionUp == UP)
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







