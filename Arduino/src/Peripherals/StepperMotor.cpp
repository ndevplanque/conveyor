#include "StepperMotor.h"

// Constructor to initialize with the I2C address
StepperMotor::StepperMotor(int movement, int speed, uint8_t i2cAddress)
    : movement(movement), speed(speed), _grbl(i2cAddress), _i2cAddress(i2cAddress)
{
    M5.begin();
    M5.Power.begin();
    _grbl.Init(&Wire);
    _grbl.setMode("distance");

    if (speed > 1000)
    {
        this->speed = 1000;
    }
}

// Make the motor move
void StepperMotor::move(bool forward)
{
    int angle = forward ? movement : -movement;
    _grbl.setMotor(angle, angle, angle, speed);
}
