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

void elev_run(void) {    
    elevatorParameters_t l_elevParam;
    elevatorParameters_t *elevParam = &l_elevParam;
    elev_init(elevParam);
        
    while (1) {
        switch (elevParam->currentState) {
            case STATE_IDLE:
                if (elevParam->signals[SIG_HAS_ORDERS]) {
                    if (elevParam->signals[SIG_SHOULD_STOP])
                        elevParam->nextState = STATE_OPENDOOR;
                    else elevParam->nextState = STATE_DRIVE;
                }
                else elevParam->nextState = STATE_IDLE;
                break;
                
            case STATE_DRIVE:
                if (elevParam->signals[SIG_SHOULD_STOP]) elevParam->nextState = STATE_OPENDOOR;
                else elevParam->nextState = STATE_DRIVE;
                break;
            case STATE_OPENDOOR:
                if (elevParam->signals[SIG_TIMER_FINISHED]) elevParam->nextState = STATE_CLOSEDOOR;
                else elevParam->nextState = STATE_OPENDOOR;
                break;
            case STATE_CLOSEDOOR:
                if (elevParam->signals[SIG_OBSTRUCTION]) elevParam->nextState = STATE_OPENDOOR;
                else if (elevParam->signals[SIG_HAS_ORDERS]) elevParam->nextState = STATE_DRIVE;
                else elevParam->nextState = STATE_IDLE;
                break;
            case STATE_EMERGENCYSTOP:
                if (elevParam->signals[SIG_HAS_ORDERS]) elevParam->nextState = STATE_CLOSEDOOR; // Making sure doors are closed and everything's in order before continuing.
                else elevParam->nextState = STATE_EMERGENCYSTOP;
                break;
        }
        
        if (elevParam->signals[SIG_EMERGENCY_STOP]) elevParam->nextState = STATE_EMERGENCYSTOP;
        
        if (elevParam->nextState != elevParam->currentState) {
            switch (elevParam->nextState) {
                case STATE_IDLE:
                    break;
                case STATE_DRIVE:
                    elev_setDirectionAndSpeed(elevParam);
                    break;
                case STATE_OPENDOOR:
                    if (elevParam->currentState == STATE_DRIVE) elev_stop(elevParam->directionUp);
                    oq_deleteOrderInFloor(elevParam->currentFloor);
                    panel_turnOffLightsInFloor(elevParam->currentFloor);
                    panel_setDoorOpenLamp(LAMP_ON);
                    timer_start();
                    break;
                case STATE_CLOSEDOOR:
                    if (elevParam->currentState == STATE_EMERGENCYSTOP) panel_setStopLamp(LAMP_OFF);
                    panel_setDoorOpenLamp(LAMP_OFF);
                    break;
                case STATE_EMERGENCYSTOP:
                    if (elevParam->currentState == STATE_DRIVE)
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
        printStatus(elevParam);
    }
}

int elev_init(elevatorParameters_t *param) {
    // Init hardware
    if (!io_init())
        return 0;
    
    if (!oq_init())
        return 0;
    if (!panel_init())
        return 0;
    elev_setSpeed(100);
    while (panel_getFloorSensorSignal()== -1) {
        ;
    }
    elev_stop(DIR_UP);
    param->currentState = STATE_IDLE;
    param->nextState = STATE_IDLE;
    int i;
    for (i=0; i<SIG_NUMBER_OF_SIGNALS; i++)
        param->signals[i] = 0;
    param->directionUp = DIR_UP;
    param->currentFloor = panel_getFloorSensorSignal();
    panel_setFloorIndicator(param->currentFloor);
    return 1;
}

void elev_updateSignals(elevatorParameters_t *param) {
    switch(param->currentState) {
        case STATE_IDLE:
            param->signals[SIG_HAS_ORDERS] = oq_hasOrders();
            param->signals[SIG_SHOULD_STOP] = (oq_hasOrderInFloor(DIR_UP, param->currentFloor) || oq_hasOrderInFloor(DIR_DOWN, param->currentFloor));
            break;
        case STATE_DRIVE:;
            int tempFloor = panel_getFloorSensorSignal(); // Had to do it, sometimes currentFloor got -1 ?!?!!??!!?
            if (tempFloor != -1) {
                param->currentFloor = tempFloor;
                panel_setFloorIndicator(param->currentFloor);
                if (oq_hasOrderInFloor(param->directionUp, param->currentFloor) || (elev_findDirection(param->currentFloor, param->directionUp) == !param->directionUp))
                    param->signals[SIG_SHOULD_STOP] = 1;
                else param->signals[SIG_SHOULD_STOP] = 0;
            }
            else param->signals[SIG_SHOULD_STOP] = 0;
            break;
        case STATE_OPENDOOR:
            param->signals[SIG_TIMER_FINISHED] = timer_timerIsFinished(3);
            break;
        case STATE_CLOSEDOOR:
            param->signals[SIG_HAS_ORDERS] = oq_hasOrders();
            param->signals[SIG_OBSTRUCTION] = panel_getObstructionSignal();
            break;
        case STATE_EMERGENCYSTOP:;
            tempFloor = panel_getFloorSensorSignal();
            if (tempFloor != -1) {
                param->currentFloor = tempFloor;
                panel_setFloorIndicator(param->currentFloor);
            }
            param->signals[SIG_HAS_ORDERS] = oq_hasOrders();
            break;
    }
    param->signals[SIG_EMERGENCY_STOP] = panel_getStopSignal();
}

elevatorDirection_t elev_findDirection(int currentFloor, int direction) {
    int floor = currentFloor;
    if (direction == DIR_UP) {
        for (floor = floor+1; floor<N_FLOORS; floor++) {
            if (oq_hasOrderInFloor(DIR_UP, floor) || oq_hasOrderInFloor(DIR_DOWN, floor))
                return DIR_UP;
        }
        return DIR_DOWN;
    }
    else if (direction == DIR_DOWN) {
        for (floor = floor-1; floor>=0; floor--) {
            if (oq_hasOrderInFloor(DIR_UP, floor) || oq_hasOrderInFloor(DIR_DOWN, floor))
                return DIR_DOWN;
        }
        return DIR_UP;
    }
    printf("orderqueue error: direction neither up nor down");
    return DIR_UP;
}

void elev_stop(elevatorDirection_t direction){
    if (direction == DIR_UP) elev_setSpeed(-100);
    else elev_setSpeed(100);
    usleep(50000);
    elev_setSpeed(0);
}

void printStatus (const elevatorParameters_t *param) {
    printf("Current Floor: %i, HasOrders: %i, directionUP: %i, currentState = %i, elevParam->nextState = %i\n", param->currentFloor, oq_hasOrders(), param->directionUp, param->currentState, param->nextState);
}


void elev_setDirectionAndSpeed(elevatorParameters_t *param) {
    param->directionUp = elev_findDirection(param->currentFloor, param->directionUp);
    if (param->directionUp == DIR_UP)
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







