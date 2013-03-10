#pragma once

/**
 Turn door-open lamp on or off.
 @param value Non-zero value turns lamp on, 0 turns lamp off.
 */
void panel_setDoorOpenLamp(int value);

/**
 Checking for incoming orders. Telling the orderqueue if any buttons are clicked.
 **/
void panel_checkForOrders(void);

/**
 Get signal from obstruction switch.
 @return 1 if obstruction is enabled. 0 if not.
 */
int panel_getObstructionSignal(void);

/**
 Get signal from stop button.
 @return 1 if stop button is pushed, 0 if not.
 */
int panel_getStopSignal(void);

/**
 Turn stop lamp on or off.
 @param value Non-zero value turns lamp on, 0 turns lamp off.
 */
void panel_setStopLamp(int value);

/**
 Get floor sensor signal.
 @return -1 if elevator is not on a floor. 0-3 if elevator is on floor. 0 is
 ground floor, 3 is top floor.
 */
int panel_getFloorSensorSignal(void);

/**
 Set floor indicator lamp for a given floor.
 @param floor Which floor lamp to turn on. Other floor lamps are turned off.
 */
void panel_setFloorIndicator(int floor);

/**
 Gets a button signal.
 @param button Which button type to check. Can be BUTTON_CALL_UP,
 BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside the elevator).
 @param floor Which floor to check button. Must be 0-3.
 @return 0 if button is not pushed. 1 if button is pushed.
 */
int panel_getButtonSignal(elev_button_type_t button, int floor);

/**
 Set a button lamp.
 @param lamp Which type of lamp to set. Can be BUTTON_CALL_UP,
 BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside" the elevator).
 @param floor Floor of lamp to set. Must be 0-3
 @param value Non-zero value turns lamp on, 0 turns lamp off.
 */
void panel_setButtomLamp(elev_button_type_t button, int floor, int value);

/**
 Updating everything on the panel.
 **/
void panel_updatePanel(void);

/**
 Turning off all panel-light in a given floor.
 **/
void panel_turnOffLightsInFloor(int);

/**
 Turning off all lights
 **/
void panel_turnOffAllLights(void);
