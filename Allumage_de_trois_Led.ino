#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Configuration Wi-Fi
const char* ssid = "Mr. Begin";
const char* password = "senoujudith";

// Configuration API YoupiLab (HTTP)
const char* apiId = "allc8424";
const char* apiKey = "13b55d3c";
const char* serverUrl = "http://iot.youpilab.com/api"; 

// Broches ESP8266
const int soundPin = 5;  // GPIO5 (D1)
const int redLed = 4;    // GPIO4 (D2)
const int greenLed = 14; // GPIO14 (D5)
const int yellowLed = 12;  // GPIO12 (D6)

const unsigned long waitTime = 1000; 

// Variables
int clapCount = 0; 
unsigned long firstClapTime = 0; 

void setup() 
{ 
  pinMode(soundPin, INPUT); 
  pinMode(redLed, OUTPUT);    
  pinMode(greenLed, OUTPUT);    
  pinMode(yellowLed, OUTPUT); 

  Serial.begin(115200); 
  Serial.println("\n=== Connexion Wi-Fi... ==="); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n=== Connecte au Wi-Fi ! ==="); 
} 

void loop() 
{ 
  // Détection d'un claquement 
  if (digitalRead(soundPin) == HIGH) 
  { 
    if (clapCount == 0) 
    { 
      firstClapTime = millis(); 
    } 

    clapCount++; 
    delay(250); 
  } 

  // Analyse des claquements après 1 seconde 
  if (clapCount > 0 && (millis() - firstClapTime >= waitTime)) 
  { 
    digitalWrite(redLed, LOW);     
    digitalWrite(greenLed, LOW);     
    digitalWrite(yellowLed, LOW); 

    String eventType = "";

    if (clapCount == 1) 
    { 
      digitalWrite(redLed, HIGH); 
      eventType = "LED_Rouge";
      Serial.println("Notification : LED Rouge allumee"); 
    } 
    else if (clapCount == 2) 
    { 
      digitalWrite(greenLed, HIGH); 
      eventType = "LED_Verte";
      Serial.println("Notification : LED Verte allumee"); 
    } 
    else if (clapCount == 3) 
    { 
      digitalWrite(yellowLed, HIGH); 
      eventType = "LED_yellow";
      Serial.println("Notification : LED Jaune allumee"); 
    } 
    else 
    { 
      eventType = "Invalide";
      Serial.println("Notification : Nombre de claquements invalide"); 
NOUVEAU CROQUIS

    } 

    // Envoi de la valeur vers YoupiLab
    sendDataToDatabase(clapCount, eventType);

    clapCount = 0; 
  } 
} 

// Fonction d'envoi vers l'API YoupiLab
void sendDataToDatabase(int claps, String action) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    if (http.begin(client, serverUrl)) {
      // YoupiLab attend un en-tête JSON
      http.addHeader("Content-Type", "application/json");

      // Payload JSON formaté pour YoupiLab (value prend le nombre de claquements)
      String jsonPayload = "{\"api_id\":\"" + String(apiId) + 
                           "\",\"api_key\":\"" + String(apiKey) + 
                           "\",\"value\":\"" + String(claps) + 
                           "\",\"action\":\"" + action + "\"}";

      int httpResponseCode = http.POST(jsonPayload);

      if (httpResponseCode > 0) {
        Serial.print("Code HTTP : ");
        Serial.println(httpResponseCode);
        
        // Affiche la réponse exacte de la plateforme
        String response = http.getString();
        Serial.print("Reponse du serveur : ");
        Serial.println(response);
      } else {
        Serial.print("Erreur HTTP : ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }

      http.end();
    } else {
      Serial.println("Erreur : Connexion HTTP impossible");
    }
  } else {
    Serial.println("Erreur : Wi-Fi deconnecte");
  }
}