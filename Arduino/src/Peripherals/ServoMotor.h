#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include <M5Stack.h>
#include <Arduino.h>
#include "GoPlus2.h"
#include "../IHM/Screen.h"

class ServoMotor
{
public:
    ServoMotor();
    void move(int angle = 0); // Faire bouger le servomoteur à un angle spécifique (0-180)
    void goToWarehouse(int warehouseId);

private:
    int angleA = 25;
    int angleB = 40;
    int angleC = 55;
    GoPlus2 goPlus;
};

#endif
