#pragma once

#define N_FLOORS 4

/**
 The different states of the statemachine
 **/
typedef enum {
    STATE_IDLE = 0,
    STATE_DRIVE,
    STATE_OPENDOOR,
    STATE_CLOSEDOOR,
    STATE_EMERGENCYSTOP
} elevatorState_t;

/**
 The different signals used by the statemachine
 **/

typedef enum {
    SIG_HAS_ORDERS,
    SIG_SHOULD_STOP,
    SIG_EMERGENCY_STOP,
    SIG_TIMER_FINISHED,
    SIG_OBSTRUCTION,
    SIG_NUMBER_OF_SIGNALS
} stateSignals_t;

/**
 Lamp on off
**/

typedef enum {
    LAMP_OFF = 0,
    LAMP_ON
} lampButtonToggle_t;

/**
 The direction of the elevator
 **/

typedef enum {
    DIR_DOWN = 0,
    DIR_UP
} elevatorDirection_t;

/**
 A struct which contains all the parameters that the elevator keeps track off
 **/

typedef struct tag_elevatorParameters_t {
    elevatorState_t currentState;
    elevatorState_t nextState;
    elevatorDirection_t directionUp;
    int currentFloor;
    int signals[sigNUMBER_OF_SIGNALS];
} elevatorParameters_t;


/**
 Button types for function elev_set_button_lamp() and elev_get_button().
 */
typedef enum tag_elev_lamp_type {
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elev_button_type_t;
