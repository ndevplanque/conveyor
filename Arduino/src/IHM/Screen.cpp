#include "Screen.h"

Screen::Screen(const bool debugMode) : debugMode(debugMode)
{
    M5.begin();
    M5.Lcd.clear(); // Efface l'écran au démarrage

    print(debugMode ? "Starting in debug mode..." : "Starting...");
}

String Screen::getUptime()
{
    unsigned long uptime = millis();

    unsigned long seconds = (uptime / 1000) % 60;
    unsigned long minutes = (uptime / (1000 * 60)) % 60;

    char buffer[6];
    sprintf(buffer, "%02lu:%02lu ", minutes, seconds);

    return String(buffer);
}

void Screen::print(String message)
{
    String text = this->getUptime() + message;

    // Découper la chaîne en plusieurs parties si elle dépasse 53 caractères
    int start = 0;
    while (start < text.length())
    {
        String part = text.substring(start, start + 53);

        if (currentLine < MAX_LINES)
        {
            lines[currentLine] = part;
            currentLine++;
        }
        else
        {
            // Déplacer toutes les lignes vers le haut si on atteint le max de lignes
            for (int i = 1; i < MAX_LINES; i++)
            {
                lines[i - 1] = lines[i];
            }
            lines[MAX_LINES - 1] = part; // Ajouter la partie de la chaîne découpée
        }
        start += 53; // Passer à la prochaine section de la chaîne
    }

    // Réafficher tout le contenu à chaque ajout
    redraw();
}

void Screen::print(String message, ErrorCode error)
{
    this->print(translateErrorCode(error) + " " + message);
}

void Screen::debug(String message)
{
    if (this->debugMode)
    {
        this->print(message);
    }
}

void Screen::debug(String message, ErrorCode error)
{
    if (this->debugMode)
    {
        this->print(message, error);
    }
}

void Screen::clear()
{
    M5.Lcd.clear();
}

// Méthode pour redessiner l'écran avec les lignes de texte actuelles
void Screen::redraw()
{
    M5.Lcd.fillRect(0, 0, 320, 200, TFT_BLACK);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextSize(1);

    // Afficher chaque ligne
    for (int i = 0; i < currentLine; i++)
    {
        M5.Lcd.setCursor(0, i * 10);
        M5.Lcd.println(lines[i]);
    }
}

void Screen::drawButtons(ConveyorMode mode)
{
    M5.Lcd.fillRect(0, 200, 320, 40, TFT_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 200);
    M5.Lcd.print("--------------------------");
    M5.Lcd.setCursor(0, 220);
    if (mode == IDLE)
    {
        M5.Lcd.print("   BACK    PROD    FWRD   ");
    }
    if (mode == BACKWARD)
    {
        M5.Lcd.print("   IDLE    PROD    FWRD   ");
    }
    if (mode == PRODUCTION)
    {
        M5.Lcd.print("   BACK    IDLE    FWRD   ");
    }
    if (mode == FORWARD)
    {
        M5.Lcd.print("   BACK    PROD    IDLE   ");
    }
}