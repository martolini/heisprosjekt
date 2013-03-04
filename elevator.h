
typedef enum {
  IDLE = 0,
  DRIVE,
  OPENDOOR,
  CLOSEDOOR,
  EMERGENCYSTOP
} elevatorState;



void updateSignals(elevatorState curState);
void setSpeed(void);
void initElevator(void);
void setDirection();
void stopElevator(void);
int getCurrentFloor(void);
void printStatus();
