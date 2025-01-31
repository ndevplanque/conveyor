#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <M5Stack.h>
#include "ConveyorMode.h"
#include "../Errors/ErrorCode.h"

class Screen
{
public:
    Screen(const bool debugMode);
    void print(String message);
    void print(String message, ErrorCode error);
    void debug(String message);
    void debug(String message, ErrorCode error);
    void error(String message);
    void error(String message, ErrorCode error);
    void drawButtons(ConveyorMode mode);

private:
    const bool debugMode;
    String getUptime();
    static const int MAX_LINES = 20; // Maximum de lignes à afficher à l'écran
    String lines[MAX_LINES];         // Tableau pour stocker les lignes de texte
    int currentLine = 0;             // Index de la ligne actuelle
    void redraw();                   // Méthode pour redessiner l'écran
};

#endif
