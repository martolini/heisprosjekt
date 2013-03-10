#pragma once

/**
 Constantly checking for incoming orders. Telling the orderqueue if any buttons are clicked.
 **/
void panel_checkForOrders(void);

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
