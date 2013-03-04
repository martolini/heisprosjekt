#pragma once

enum {
  DOWN = 0,
  UP = 1
};

/**
   Initializing the queue, sets all orders to 0
**/
void initQueue();

/**
   Checking if the elevator has orders 
   @return 1 if it has orders, 0 otherwise.
**/
int hasOrders();

/**
   Adding order from the panel
   @param floor The floor which the order is recieved
**/
void addOuterOrder(int, int);
void addInnerOrder(int, int);
void deleteOrderInFloor(int);
int hasOrderInFloor(int, int);
void deleteAllOrders();

