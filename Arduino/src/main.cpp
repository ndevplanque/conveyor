#include <Arduino.h>
#include "DolibarrFacade.h"

const char* ssid = "Delplanque";
const char* password = "nicolebg67$$1234++";

DolibarrFacade *dolibarr;
String dolip = "http://172.20.10.8:8080";
String dolapipath = "/dolibarr/api/index.php";
String dolapikey = "4eS9aC4829OLrfY3yRsXtaIHg6zK6Kz0";

void setup() {
    Serial.begin(115200);

    dolibarr = new DolibarrFacade(dolip + dolapipath, dolapikey);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connexion au Wi-Fi...");
    }
    Serial.println("Connecté au Wi-Fi !");
}

void loop() {
    ErrorCode result = dolibarr->addStockMovementByRef("VERT", 1);

    // Vérifier le résultat de l'opération
    if (result == SUCCESS) {
        Serial.println("Mouvement de stock ajouté avec succès !");
    } else {
        Serial.print("Erreur lors de l'ajout du mouvement de stock. Code erreur: ");
        Serial.println(result);
    }

    delay(10000);  // Pause avant la prochaine requête
}