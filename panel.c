#include <stdio.h>
#include "elev.h"
#include "orderqueue.h"
#include "elevator.h"
#include "panel.h"


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
            int buttonPushed = elev_get_button_signal(buttonType, floor);
            if (buttonPushed) {
                elev_set_button_lamp(buttonType, floor, LAMP_ON);
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
        elev_set_button_lamp(buttonType, floor, LAMP_OFF);
    }
}

void panel_turnOffAllLights(void) {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++)
        panel_turnOffLightsInFloor(floor);
}
