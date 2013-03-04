#pragma once

enum {
  DOWN = 0,
  UP = 1
};

/**
   Initializing the queue, sets all orders to 0
 @return Nonzero on success, 0 if failure
**/
int initQueue(void);

/**
   Checking if the elevator has orders 
   @return 1 if it has orders, 0 otherwise.
**/
int hasOrders(void);

/**
   Adding order from the panel
   @param1 direction The direction the order is in (user pressed up/down)
   @param2 floor The floor from which the order is recieved
**/
void addOuterOrder(int, int);

/**
 Adding order from inside the elevator
 @param1 floor The floor-button the user push
 @param2 currentFloor The current floor of the elevator, used for calculating direction
 **/
void addInnerOrder(int, int);

/**
 Deleting an order in a given floor from the queue, used when the elevator stops
 @param floor The floor which should be removed from the queue
 **/
void deleteOrderInFloor(int);

/**
 Checking if the elevator has an order in a given floor and direction.
 @param1 direction Whether the user is going up or down
 @param2 floor The endpoint of the order
 **/
int hasOrderInFloor(int, int);

/**
 Deleting all orders, used for the state EMERGENCYSTOP.
 **/
void deleteAllOrders(void);

