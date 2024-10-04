#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "Logger.h"

class ServoMotor
{
public:
    ServoMotor(Logger *logs, int pin);

    void attach();
    void moveToAngle(int angle);
    void detach();

    int getCurrentAngle();

private:
    Servo myServo;
    Logger *logs;
    int servoPin;
    int currentAngle;
};

#endif
