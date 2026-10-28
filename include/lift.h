#ifndef _LIFT_H_
#define _LIFT_H_

// heights are inches above the starting position

void liftInit();
void setLiftHeight(float inches);
void moveLiftBy(float inches);
void liftManual(int power);
float getLiftHeight();
float getLiftTarget();
bool liftAtTarget();
void waitUntilLiftSettled(int timeoutMs = 2000);

#endif
