#include "orderqueue.h"
#include "elev.h"
#include <stdio.h>

/**
 Defining the orderqueue, twodimensional array with UP and DOWN (enum) as indexes.
 **/
static int orders[2][N_FLOORS];

int initQueue() {
    static int isInitialized = 0;
    if (!isInitialized) {
        int dir, floor;
        for (dir=DOWN; dir<= UP; dir++) {
            for (floor=0; floor<N_FLOORS; floor++)
                orders[dir][floor] = 0;
        }
        isInitialized = 1;
    }
    return isInitialized;
}


int hasOrders() {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++) {
        if (orders[DOWN][floor] || orders[UP][floor])
            return 1;
    }
    return 0;
}

void addOuterOrder(int direction, int floor) {
    orders[direction][floor] = 1;
}

void addInnerOrder(int floor, int currentFloor) {
    int direction = DOWN;
    if (floor > currentFloor)
        direction = UP;
    orders[direction][floor] = 1;
}

void deleteOrderInFloor(int floor) {
    int dir;
    for (dir=DOWN; dir<= UP; dir++)
        orders[dir][floor] = 0;
}

int hasOrderInFloor(int direction, int floor) {
    return orders[direction][floor];
}

void deleteAllOrders() {
    int dir, floor;
    for (dir=DOWN; dir<=UP; dir++) {
        for (floor=0; floor<N_FLOORS; floor++)
            orders[dir][floor] = 0;
    }
}



