#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <M5Stack.h>
#include <Arduino.h>
#include "GoPlus2.h"
#include "Logger.h"

class ServoMotor
{
public:
    ServoMotor(Logger *logger);
    void move(int angle = 0); // Faire bouger le servomoteur à un angle spécifique (0-180)

private:
    Logger *logger;
    GoPlus2 goPlus;
};

#endif
