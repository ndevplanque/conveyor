#include "ServoMotor.h"

ServoMotor::ServoMotor(Logger *logs, int pin)
    : logs(logs), servoPin(pin), currentAngle(0) {}

void ServoMotor::attach()
{
    myServo.attach(servoPin);
}

void ServoMotor::moveToAngle(int angle)
{
    if (angle >= 0 && angle <= 180)
    {
        myServo.write(angle);
        currentAngle = angle;
        logs->print("Servo moved to angle: " + String(angle));
    }
    else
    {
        logs->print("Invalid angle: " + String(angle));
    }
}

void ServoMotor::detach()
{
    myServo.detach();
}

int ServoMotor::getCurrentAngle()
{
    return currentAngle;
}
