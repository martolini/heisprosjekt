#include <assert.h>
#include <stdio.h>
#include "orderqueue.h"
#include "elevator.h"
#include "panel.h"
#include "io.h"
#include "channels.h"


void panel_checkForOrders(void) {
    int floor;
    elev_button_type_t buttonType;
    for (floor=0; floor<N_FLOORS; floor++) {
        for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
            if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1)) // There is no downbutton from bottom floor, and no upbutton for topfloor
                continue;
            else if (buttonType != BUTTON_COMMAND && getCurrentElevatorState() == EMERGENCYSTOP) // If the elevator is in EMERGENCYSTOP and order's from the outside, don't do anything
                continue;
            else if ((getCurrentElevatorState() == OPENDOOR || getCurrentElevatorState() == CLOSEDOOR) && floor == getCurrentFloor()) // If an order comes from the current floor when the elevator is in either CLOSEDOOR or OPENDOOR, don't do anything.
                continue;
            int buttonPushed = panel_getButtonSignal(buttonType, floor);
            if (buttonPushed) {
                panel_setButtonLamp(buttonType, floor, LAMP_ON);
                switch(buttonType) {
                    case BUTTON_CALL_UP:
                        oq_addOuterOrder(UP, floor);
                        break;
                    case BUTTON_CALL_DOWN:
                        oq_addOuterOrder(DOWN, floor);
                        break;
                    case BUTTON_COMMAND: // order recieved inside the elevator
                        oq_addInnerOrder(floor, getCurrentFloor());
                        break;
                }
            }
        }
    }
}

void panel_turnOffLightsInFloor(int floor) {
    elev_button_type_t buttonType;
    for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
        if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1))
            continue;
        panel_setButtonLamp(buttonType, floor, LAMP_OFF);
    }
}

void panel_turnOffAllLights(void) {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++)
        panel_turnOffLightsInFloor(floor);
}

void elev_set_door_open_lamp(int value) {
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

