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

    http->setTimeout(5000);                              // Définit un timeout de 5 secondes.
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
    HTTPClient *http = dolibarr(endpoint); // Prépare le client HTTP.

    // Construit le corps de la requête JSON.
    String payload = "{\"product_id\":" + String(productId) + ",\"warehouse_id\":" + String(warehouseId) + ",\"qty\":" + String(qty) + "}";
    int httpCode = http->POST(payload); // Envoie une requête POST avec le payload.

    // Affiche le statut de la requête.
    screen->debug(String(httpCode) + " POST " + endpoint);

    http->end(); // Termine la connexion HTTP.
    delete http; // Libère la mémoire allouée pour le client HTTP.

    // Vérifie si le code de statut HTTP indique une erreur.
    if (httpCode < 200 || httpCode >= 300)
    {
        return HTTP_REQUEST_FAILED; // Retourne une erreur si la requête a échoué.
    }

    return SUCCESS; // Retourne SUCCESS si tout s'est bien passé.
}

int DolibarrFacade::findIdByWarehouse(char warehouse)
{
    if (warehouse == 'A')
    {
        return 1;
    }
    if (warehouse == 'B')
    {
        return 2;
    }
    if (warehouse == 'C')
    {
        return 3;
    }
    return 0;
}

bool DolibarrFacade::isValidWarehouse(char warehouse)
{
    return warehouse == 'A' ||
           warehouse == 'B' ||
           warehouse == 'C';
}

bool DolibarrFacade::isOnline()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    screen->print("Connecting to Dolibarr...");

    String endpoint = "/products?properties=id,ref,label,fk_default_warehouse,stock_reel";
    HTTPClient *http = dolibarr(endpoint);

    int httpCode = http->GET(); // Envoie une requête GET au serveur.

    http->end();
    delete http;

    return httpCode == 200;
}