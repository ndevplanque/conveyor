#include "StepperMotor.h"

// Constructor to initialize with the I2C address
StepperMotor::StepperMotor(uint8_t i2cAddress) : _grbl(i2cAddress), _i2cAddress(i2cAddress)
{
    M5.begin();
    M5.Power.begin();
    _grbl.Init(&Wire);
    _grbl.setMode("absolute");
}

// Control the motor based on button inputs
void StepperMotor::turn(int x)
{
    _grbl.setMotor(x);
}
