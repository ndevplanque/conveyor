#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>
#include <M5Stack.h>
#include <Module_GRBL_13.2.h>
#include "../IHM/Screen.h"

class StepperMotor
{
public:
    StepperMotor(int movement, int speed, uint8_t i2cAddress = 0x70);
    void move(bool forward);

private:
    int movement;
    int speed;
    Module_GRBL _grbl;
    uint8_t _i2cAddress;
};

#endif
