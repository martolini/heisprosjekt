#include "orderqueue.h"
#include <stdio.h>

/**
 Defining the orderqueue, twodimensional array with DIR_UP and DIR_DOWN (enum) as indexes.
 **/
static int orders[2][N_FLOORS];

int oq_init(void) {
    static int isInitialized = 0;
    // Setting all orders to 0 (no order).
    if (!isInitialized) {
        int dir, floor;
        for (dir=DIR_DOWN; dir<= DIR_UP; dir++) {
            for (floor=0; floor<N_FLOORS; floor++)
                orders[dir][floor] = 0;
        }
        isInitialized = 1;
    }
    return isInitialized;
}


int oq_hasOrders(void) {
    int floor;
    for (floor = 0; floor<N_FLOORS; floor++) {
        if (orders[DIR_DOWN][floor] || orders[DIR_UP][floor])
            return 1;
    }
    return 0;
}

void oq_addOuterOrder(elevatorDirection_t direction, int floor) {
    orders[direction][floor] = 1;
}

void oq_addInnerOrder(int floor, int currentFloor) {
    int direction = DIR_DOWN;
    if (floor > currentFloor)
        direction = DIR_UP;
    orders[direction][floor] = 1;
}

void oq_deleteOrderInFloor(int floor) {
    int dir;
    for (dir=DIR_DOWN; dir<= DIR_UP; dir++)
        orders[dir][floor] = 0;
}

int oq_hasOrderInFloor(elevatorDirection_t direction, int floor) {
    return orders[direction][floor];
}

void oq_deleteAllOrders(void) {
    int dir, floor;
    for (dir=DIR_DOWN; dir<=DIR_UP; dir++) {
        for (floor=0; floor<N_FLOORS; floor++)
            orders[dir][floor] = 0;
    }
}



