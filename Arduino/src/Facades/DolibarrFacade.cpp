#include "DolibarrFacade.h"

// Constructeur de la classe DolibarrFacade
// Initialise les membres avec les valeurs passées en paramètres.
DolibarrFacade::DolibarrFacade(Screen *screen, const char *dolApiRoot, const char *dolApiPath, const char *dolApiKey)
    : screen(screen), dolApiUrl(String(dolApiRoot) + String(dolApiPath)), dolApiKey(String(dolApiKey))
{
}

// Méthode pour préparer une requête HTTP vers l'API Dolibarr
// Prend un endpoint en paramètre et configure les en-têtes nécessaires.
HTTPClient *DolibarrFacade::dolibarr(String endpoint)
{
    HTTPClient *http = new HTTPClient(); // Crée un client HTTP.

    String url = this->dolApiUrl + endpoint; // Construit l'URL complète avec le endpoint.
    http->begin(url);                        // Initialise la connexion à l'URL.

    http->setTimeout(30000);                             // Définit un timeout de 30 secondes.
    http->addHeader("Content-Type", "application/json"); // Ajoute l'en-tête Content-Type.
    http->addHeader("DOLAPIKEY", this->dolApiKey);       // Ajoute l'en-tête contenant la clé API.

    return http; // Retourne le client HTTP configuré.
}

// Méthode pour ajouter un mouvement de stock basé sur la référence d'un produit
ErrorCode DolibarrFacade::addStockMovement(int productId, int warehouseId, int qty)
{
    // Vérifie si le WiFi est connecté.
    if (WiFi.status() != WL_CONNECTED)
    {
        return WIFI_NOT_CONNECTED; // Retourne une erreur si le WiFi est déconnecté.
    }

    // Construit le endpoint pour les mouvements de stock.
    String endpoint = "/stockmovements";
    // Construit le corps de la requête JSON.
    String payload = "{\"product_id\":" + String(productId) + ",\"warehouse_id\":" + String(warehouseId) + ",\"qty\":" + String(qty) + "}";

    HTTPClient *http = dolibarr(endpoint); // Prépare le client HTTP.
    int httpCode = http->POST(payload);    // Envoie une requête POST avec le payload.

    // Affiche le statut de la requête.
    screen->print(String(httpCode) + " POST " + endpoint);

    http->end(); // Termine la connexion HTTP.
    delete http; // Libère la mémoire allouée pour le client HTTP.

    // Vérifie si le code de statut HTTP indique une erreur.
    if (httpCode < 200 || httpCode >= 300)
    {
        return HTTP_REQUEST_FAILED; // Retourne une erreur si la requête a échoué.
    }

    return SUCCESS; // Retourne SUCCESS si tout s'est bien passé.
}

// Méthode pour récupérer les données d'un produit par sa référence
ErrorCode DolibarrFacade::getProductDataByRef(String productRef, JsonDocument &doc)
{
    // Vérifie si le WiFi est connecté.
    if (WiFi.status() != WL_CONNECTED)
    {
        return WIFI_NOT_CONNECTED; // Retourne une erreur si le WiFi est déconnecté.
    }

    // Construit le endpoint pour récupérer les données du produit.
    String endpoint = "/products?sqlfilters=t.ref:=:'" + productRef + "'&properties=id,fk_default_warehouse";

    HTTPClient *http = dolibarr(endpoint); // Prépare le client HTTP.
    int httpCode = http->GET();            // Envoie une requête GET au serveur.

    // Affiche le statut de la requête.
    screen->print(String(httpCode) + " GET " + endpoint);

    // Vérifie si le code de statut HTTP indique une erreur.
    if (httpCode < 200 || httpCode >= 300)
    {
        http->end();                // Termine la connexion HTTP.
        delete http;                // Libère la mémoire allouée pour le client HTTP.
        return HTTP_REQUEST_FAILED; // Retourne une erreur si la requête a échoué.
    }

    String payload = http->getString(); // Récupère la réponse JSON sous forme de chaîne.
    http->end();                        // Termine la connexion HTTP.
    delete http;                        // Libère la mémoire allouée pour le client HTTP.

    // Désérialise la chaîne JSON dans le document JSON.
    DeserializationError error = deserializeJson(doc, payload);

    // Vérifie si la désérialisation a échoué.
    if (error)
    {
        return JSON_PARSING_FAILED; // Retourne une erreur si le parsing a échoué.
    }

    return SUCCESS; // Retourne SUCCESS si tout s'est bien passé.
}

String DolibarrFacade::translateToRef(String rfidScan)
{
    if (rfidScan == "fd e3 d6 df")
    {
        return "VERT";
    }
    if (rfidScan == "bd e4 d6 df")
    {
        return "JAUNE";
    }
    if (rfidScan == "3d e4 d6 df")
    {
        return "BLEU";
    }
    if (rfidScan == "8d dd d6 df")
    {
        return "ROUGE";
    }
    return rfidScan;
}

bool DolibarrFacade::isValidProductRef(String ref)
{
    return ref == "VERT" ||
           ref == "BLEU" ||
           ref == "JAUNE";
}