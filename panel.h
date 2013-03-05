#pragma once

/**
 Constantly checking for incoming orders. Telling the orderqueue if any buttons are clicked.
 **/
void checkForOrders(void);

/**
 Updating everything on the panel.
 **/
void updatePanel(void);

/**
 Turning off all panel-light in a given floor.
 **/
void turnOffLightsInFloor(int);

/**
 Turning on floor light indicator.
 **/
void turnOnFloorLightIndicator(int);

/**
 Turning off all lights
 **/
void turnOffAllLights(void);
