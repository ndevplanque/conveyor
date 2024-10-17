#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>
#include <M5Stack.h>
#include <Module_GRBL_13.2.h>
#include "Logger.h"

class StepperMotor
{
public:
    StepperMotor(uint8_t i2cAddress = 0x70);
    void init();
    void turn(int x = 0);

private:
    Module_GRBL _grbl;
    uint8_t _i2cAddress;
};

#endif
