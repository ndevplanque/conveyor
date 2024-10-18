#include "ServoMotor.h"

// Constructeur par défaut
ServoMotor::ServoMotor(Logger *logger)
    : logger(logger)
{
    goPlus.begin();
}

// Faire bouger le servomoteur à un angle spécifique
void ServoMotor::move(int angle)
{
    // Limiter l'angle entre 0 et 180 degrés pour le servomoteur
    if (angle < 0)
    {
        angle = 0;
    }
    else if (angle > 180)
    {
        angle = 180;
    }
    goPlus.Servo_write_angle(SERVO_NUM0, uint8_t(angle));
    goPlus.Servo_write_angle(SERVO_NUM1, uint8_t(angle));
    goPlus.Servo_write_angle(SERVO_NUM2, uint8_t(angle));
    goPlus.Servo_write_angle(SERVO_NUM3, uint8_t(angle));
    logger->print("Servo " + String(angle));
}
