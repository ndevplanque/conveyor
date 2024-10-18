#include "StepperMotor.h"

// Constructor to initialize with the I2C address
StepperMotor::StepperMotor(Logger *logger, uint8_t i2cAddress)
    : logger(logger), _grbl(i2cAddress), _i2cAddress(i2cAddress)
{
    M5.begin();
    M5.Power.begin();
    _grbl.Init(&Wire);
    _grbl.setMode("absolute");
}

// Control the motor based on button inputs
void StepperMotor::move(int angle, int speed)
{
    _grbl.setMotor(angle, angle, angle, speed);
    logger->print("Stepper " + String(angle) + "|" + String(speed));
}
