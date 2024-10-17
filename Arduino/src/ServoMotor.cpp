#include "ServoMotor.h"

ServoMotor::ServoMotor(Logger *logger, int pin)
    : logger(logger), servoPin(pin), currentAngle(0) {}

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
        logger->print("Servo moved to angle: " + String(angle));
    }
    else
    {
        logger->print("Invalid angle: " + String(angle));
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
