#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>
#include <Stepper.h>
#include "Logger.h"

class StepperMotor
{
public:
    StepperMotor(Logger *logs, int stepsPerRevolution, int pin1, int pin2, int pin3, int pin4);

    void attach();
    void moveSteps(int steps);
    void setSpeed(int speedRPM);

private:
    Stepper stepper;
    Logger *logs;
    int stepsPerRevolution;
    int pin1, pin2, pin3, pin4;
};

#endif
