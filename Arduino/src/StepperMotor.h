#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>
#include <M5Stack.h>
#include <Module_GRBL_13.2.h>
#include "Screen.h"

class StepperMotor
{
public:
    StepperMotor(Screen *screen, uint8_t i2cAddress = 0x70);
    void move(int angle = 0, int speed = 300);
    void waitIdle();
    bool isIdle();

private:
    Screen *screen;
    Module_GRBL _grbl;
    uint8_t _i2cAddress;
};

#endif
