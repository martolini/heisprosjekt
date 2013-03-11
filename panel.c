#include <assert.h>
#include <stdio.h>
#include "orderqueue.h"
#include "elevator.h"
#include "panel.h"
#include "io.h"
#include "channels.h"


// Number of signals and lamps on a per-floor basis (excl sensor)
#define N_BUTTONS 3


// Matrix of lamp channels indexed by floor and button type. Excuse ugly macro.
#define LCM_SET(f) {LIGHT##f##_UP, LIGHT##f##_DOWN, LIGHT##f##_COMMAND}
static const int lamp_channel_matrix[N_FLOORS][N_BUTTONS] =
{LCM_SET(1), LCM_SET(2), LCM_SET(3), LCM_SET(4)};


// Matrix of elevator button signal channels indexed by floor and button type.
#define SCM_SET(f) { FLOOR##f##_UP, FLOOR##f##_DOWN, FLOOR##f##_COMMAND }
static const int button_channel_matrix[N_FLOORS][N_BUTTONS] =
{SCM_SET(1), SCM_SET(2), SCM_SET(3), SCM_SET(4)};

int panel_init(void) {
    int i;
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
    return 1;
}

void panel_checkForOrders(const elevatorParameters_t *param) {
    int floor;
    elev_button_type_t buttonType;
    for (floor=0; floor<N_FLOORS; floor++) {
        for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
            if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1)) // There is no downbutton from bottom floor, and no upbutton for topfloor
                continue;
            else if (buttonType != BUTTON_COMMAND && param->currentState == STATE_EMERGENCYSTOP) // If the elevator is in STATE_EMERGENCYSTOP and order's from the outside, don't do anything
                continue;
            else if ((param->currentState == STATE_OPENDOOR || param->currentState == STATE_CLOSEDOOR) && floor == param->currentFloor) // If an order comes from the current floor when the elevator is in either STATE_CLOSEDOOR or STATE_OPENDOOR, don't do anything.
                continue;
            int buttonPushed = panel_getButtonSignal(buttonType, floor);
            if (buttonPushed) {
                panel_setButtonLamp(buttonType, floor, LAMP_ON);
                switch(buttonType) {
                    case BUTTON_CALL_UP:
                        oq_addOuterOrder(DIR_UP, floor);
                        break;
                    case BUTTON_CALL_DOWN:
                        oq_addOuterOrder(DIR_DOWN, floor);
                        break;
                    case BUTTON_COMMAND: // order recieved inside the elevator
                        oq_addInnerOrder(floor, param->currentFloor);
                        break;
                }
            }
        }
    }
}

void panel_turnOffLightsInFloor(int floor) {
    elev_button_type_t buttonType;
    for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
        if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1)) // Making sure we don't get button down in ground floor and button up in top floor
            continue;
        panel_setButtonLamp(buttonType, floor, LAMP_OFF);
    }
}

void panel_turnOffAllLights(void) {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++)
        panel_turnOffLightsInFloor(floor);
}

void panel_setDoorOpenLamp(int value) {
    if (value)
        io_set_bit(DOOR_OPEN);
    else
        io_clear_bit(DOOR_OPEN);
}



int panel_getObstructionSignal(void) {
    return io_read_bit(OBSTRUCTION);
}



int panel_getStopSignal(void) {
    return io_read_bit(STOP);
}



void panel_setStopLamp(int value) {
    if (value)
        io_set_bit(LIGHT_STOP);
    else
        io_clear_bit(LIGHT_STOP);
}



int panel_getFloorSensorSignal(void) {
    if (io_read_bit(SENSOR1))
        return 0;
    else if (io_read_bit(SENSOR2))
        return 1;
    else if (io_read_bit(SENSOR3))
        return 2;
    else if (io_read_bit(SENSOR4))
        return 3;
    else
        return -1;
}



void panel_setFloorIndicator(int floor) {
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    
    if (floor & 0x02)
        io_set_bit(FLOOR_IND1);
    else
        io_clear_bit(FLOOR_IND1);
    
    if (floor & 0x01)
        io_set_bit(FLOOR_IND2);
    else
        io_clear_bit(FLOOR_IND2);
}



int panel_getButtonSignal(elev_button_type_t button, int floor) {
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS-1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button ==
           BUTTON_COMMAND);
    
    if (io_read_bit(button_channel_matrix[floor][button]))
        return 1;
    else
        return 0;
}



void panel_setButtonLamp(elev_button_type_t button, int floor, int value) {
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS-1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button ==
           BUTTON_COMMAND);
    
    if (value == 1)
        io_set_bit(lamp_channel_matrix[floor][button]);
    else
        io_clear_bit(lamp_channel_matrix[floor][button]);
}

