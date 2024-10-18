#include "Logger.h"

// Tableau statique de chaînes correspondant aux valeurs de l'enum ErrorCode
const char *Logger::errorCodeStrings[] = {
    "SUCCESS",
    "WIFI_NOT_CONNECTED",
    "HTTP_REQUEST_FAILED",
    "JSON_PARSING_FAILED"};

Logger::Logger()
{
    M5.begin();
    M5.Lcd.clear();                            // Efface l'écran au démarrage
    M5.Lcd.setTextSize(1);                     // Définir la taille du texte
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK); // Texte blanc sur fond noir
    print("Démarrage...");
}

String Logger::getUptime()
{
    unsigned long uptime = millis();

    unsigned long seconds = (uptime / 1000) % 60;
    unsigned long minutes = (uptime / (1000 * 60)) % 60;

    char buffer[6];
    sprintf(buffer, "%02lu:%02lu ", minutes, seconds);

    return String(buffer);
}

void Logger::print(String message)
{
    String text = this->getUptime() + message;

    // Découper la chaîne en plusieurs parties si elle dépasse 53 caractères
    int start = 0;
    while (start < text.length()) {
        String part = text.substring(start, start + 53);
        
        if (currentLine < MAX_LINES) {
            lines[currentLine] = part;
            currentLine++;
        } else {
            // Déplacer toutes les lignes vers le haut si on atteint le max de lignes
            for (int i = 1; i < MAX_LINES; i++) {
                lines[i - 1] = lines[i];
            }
            lines[MAX_LINES - 1] = part;  // Ajouter la partie de la chaîne découpée
        }
        start += 53;  // Passer à la prochaine section de la chaîne
    }

    // Réafficher tout le contenu à chaque ajout
    redraw();
}

void Logger::print(String message, ErrorCode error)
{
    this->print(String(errorCodeStrings[error]) + " " + message);
}

void Logger::clear()
{
    M5.Lcd.clear();
}

// Méthode pour redessiner l'écran avec les lignes de texte actuelles
void Logger::redraw()
{
    M5.Lcd.fillScreen(TFT_BLACK); // Efface l'écran

    // Afficher chaque ligne
    for (int i = 0; i < currentLine; i++)
    {
        M5.Lcd.setCursor(0, i * 10);
        M5.Lcd.println(lines[i]);
    }
}
