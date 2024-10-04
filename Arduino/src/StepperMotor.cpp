#include "StepperMotor.h"

StepperMotor::StepperMotor(Logger *logs, int stepsPerRevolution, int pin1, int pin2, int pin3, int pin4)
    : logs(logs), stepper(stepsPerRevolution, pin1, pin2, pin3, pin4),
      stepsPerRevolution(stepsPerRevolution),
      pin1(pin1), pin2(pin2), pin3(pin3), pin4(pin4)
{
}

void StepperMotor::attach()
{
    logs->print("Stepper motor attached.");
}

void StepperMotor::moveSteps(int steps)
{
    stepper.step(steps);
    logs->print("Stepper motor moved " + String(steps) + " steps.");
}

void StepperMotor::setSpeed(int speedRPM)
{
    stepper.setSpeed(speedRPM);
    logs->print("Stepper motor speed set to " + String(speedRPM) + " RPM.");
}
