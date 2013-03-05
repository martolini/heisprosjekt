#pragma once

typedef enum {
    IDLE = 0,
    DRIVE,
    OPENDOOR,
    CLOSEDOOR,
    EMERGENCYSTOP
} elevatorState;

typedef enum {
    LAMP_OFF = 0,
    LAMP_ON
} lampButtonToggle;

typedef enum {
    DOWN = 0,
    UP
} elevatorDirection;