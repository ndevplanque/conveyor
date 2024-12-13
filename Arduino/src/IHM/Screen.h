#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <M5Stack.h>
#include "ConveyorMode.h"
#include "../Errors/ErrorCode.h"

class Screen
{
public:
    Screen();

    void print(String message);
    void print(String message, ErrorCode error);
    void clear();
    void drawButtons(ConveyorMode mode);

private:
    String getUptime();
    static const int MAX_LINES = 20; // Maximum de lignes à afficher à l'écran
    String lines[MAX_LINES];         // Tableau pour stocker les lignes de texte
    int currentLine = 0;             // Index de la ligne actuelle
    void redraw();                   // Méthode pour redessiner l'écran
};

#endif
