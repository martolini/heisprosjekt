#include <stdio.h>
#include "elev.h"
#include "orderqueue.h"
#include "elevator.h"
#include "panel.h"


void checkForOrders() {
    int floor;
    elev_button_type_t buttonType;
    for (floor=0; floor<N_FLOORS; floor++) {
        for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
            if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1))
                continue;
            if (buttonType != BUTTON_COMMAND && getCurrentElevatorState() == EMERGENCYSTOP)
                continue;
            int buttonPushed = elev_get_button_signal(buttonType, floor);
            if (buttonPushed) {
                elev_set_button_lamp(buttonType, floor, LAMP_ON);
                switch(buttonType) {
                    case BUTTON_CALL_UP:
                        addOuterOrder(UP, floor);
                        break;
                    case BUTTON_CALL_DOWN:
                        addOuterOrder(DOWN, floor);
                        break;
                    case BUTTON_COMMAND: // order recieved inside the elevator
                        addInnerOrder(floor, getCurrentFloor());
                        break;
                }
            }
        }
    }
}

void turnOffLightsInFloor(int floor) {
    elev_button_type_t buttonType;
    for (buttonType = BUTTON_CALL_UP; buttonType <= BUTTON_COMMAND; buttonType++) {
        if ((buttonType == BUTTON_CALL_DOWN && floor == 0) || (buttonType == BUTTON_CALL_UP && floor == N_FLOORS-1))
            continue;
        elev_set_button_lamp(buttonType, floor, LAMP_OFF);
    }
}

void updatePanel() {
    checkForOrders();
}

void turnOnFloorLightIndicator(int floor) {
    elev_set_floor_indicator(floor);
}


void turnOffAllLights() {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++)
        turnOffLightsInFloor(floor);
}
