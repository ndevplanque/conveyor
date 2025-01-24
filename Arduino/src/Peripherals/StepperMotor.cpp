#include "StepperMotor.h"

// Constructor to initialize with the I2C address
StepperMotor::StepperMotor(Screen *screen, uint8_t i2cAddress)
    : screen(screen), _grbl(i2cAddress), _i2cAddress(i2cAddress)
{
    M5.begin();
    M5.Power.begin();
    _grbl.Init(&Wire);
    _grbl.setMode("distance");
}

// Make the motor move
void StepperMotor::move(int angle, int speed)
{
    if (speed > 1000)
    {
        speed = 1000;
    }
    _grbl.setMotor(angle, angle, angle, speed);
    screen->debug("Stepper " + String(angle) + "|" + String(speed));
}

// Wait for the motor to stop
void StepperMotor::waitIdle()
{
    _grbl.waitIdle();
}

// Wait for the motor to stop
bool StepperMotor::isIdle()
{
    return _grbl.readIdle();
}