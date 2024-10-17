#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <M5Stack.h>
#include "ErrorCode.h"

class Logger
{
public:
    Logger();

    void print(String message);
    void print(String message, ErrorCode error);
    void clear();

private:
    String getUptime();
    static const char *errorCodeStrings[];
    static const int MAX_LINES = 24;  // Maximum de lignes à afficher à l'écran
    String lines[MAX_LINES];          // Tableau pour stocker les lignes de texte
    int currentLine = 0;              // Index de la ligne actuelle
    void redraw();                    // Méthode pour redessiner l'écran
};

#endif
