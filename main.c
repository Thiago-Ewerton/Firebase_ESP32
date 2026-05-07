/*
   ESP32-C3 Mini + Firebase
   ------------------------
   Botão 1 -> incrementa valor
   Botão 2 -> decrementa valor

   Envia para:
   /contador/valor

   Biblioteca:
   FirebaseClient
*/

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// ==========================
// WIFI
// ==========================
#define WIFI_SSID     "A36 de Thiago"
#define WIFI_PASSWORD "yixo6600"

// ==========================
// FIREBASE
// ==========================
#define Web_API_KEY  "AIzaSyBc_Muebv5qcDu-Se-xBYOGaSt32MYmqdE"
#define DATABASE_URL "https://projeto-esp-d6c1d-default-rtdb.firebaseio.com//"

#define USER_EMAIL "thiago.ewerton.05@gmail.com"
#define USER_PASS  "123456789"

// ==========================
// BOTÕES
// ==========================
#define BTN_INCREMENTAR 20
#define BTN_DIMINUIR   21

// ==========================
// VARIÁVEL
// ==========================
int valor = 0;

// Estados anteriores
bool ultimoEstadoInc = HIGH;
bool ultimoEstadoDec = HIGH;

// ==========================
// FIREBASE
// ==========================
void processData(AsyncResult &aResult);

UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

RealtimeDatabase Database;

// ==========================
// SETUP
// ==========================
void setup() {

  Serial.begin(115200);

  // Botões
  pinMode(BTN_INCREMENTAR, INPUT_PULLUP);
  pinMode(BTN_DIMINUIR, INPUT_PULLUP);

  // ==========================
  // WIFI
  // ==========================
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.println("WiFi conectado!");

  // ==========================
  // SSL
  // ==========================
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);

  // ==========================
  // FIREBASE
  // ==========================
  initializeApp(aClient, app, getAuth(user_auth), processData, "authTask");

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);

  Serial.println("Firebase iniciado");
}

// ==========================
// LOOP
// ==========================
void loop() {

  app.loop();

  // Só executa se autenticado
  if (app.ready()) {

    bool estadoInc = digitalRead(BTN_INCREMENTAR);
    bool estadoDec = digitalRead(BTN_DIMINUIR);

    // ==========================
    // INCREMENTAR
    // ==========================
    if (ultimoEstadoInc == HIGH && estadoInc == LOW) {

      valor++;

      Serial.print("Valor: ");
      Serial.println(valor);

      Database.set<int>(
        aClient,
        "/contador/valor",
        valor,
        processData,
        "Enviar_Incremento"
      );

      delay(200);
    }

    // ==========================
    // DIMINUIR
    // ==========================
    if (ultimoEstadoDec == HIGH && estadoDec == LOW) {

      valor--;

      Serial.print("Valor: ");
      Serial.println(valor);

      Database.set<int>(
        aClient,
        "/contador/valor",
        valor,
        processData,
        "Enviar_Decremento"
      );

      delay(200);
    }

    ultimoEstadoInc = estadoInc;
    ultimoEstadoDec = estadoDec;
  }
}

// ==========================
// CALLBACK FIREBASE
// ==========================
void processData(AsyncResult &aResult) {

  if (!aResult.isResult())
    return;

  if (aResult.isEvent()) {
    Firebase.printf(
      "Event task: %s, msg: %s, code: %d\n",
      aResult.uid().c_str(),
      aResult.eventLog().message().c_str(),
      aResult.eventLog().code()
    );
  }

  if (aResult.isDebug()) {
    Firebase.printf(
      "Debug task: %s, msg: %s\n",
      aResult.uid().c_str(),
      aResult.debug().c_str()
    );
  }

  if (aResult.isError()) {
    Firebase.printf(
      "Error task: %s, msg: %s, code: %d\n",
      aResult.uid().c_str(),
      aResult.error().message().c_str(),
      aResult.error().code()
    );
  }

  if (aResult.available()) {
    Firebase.printf(
      "task: %s, payload: %s\n",
      aResult.uid().c_str(),
      aResult.c_str()
    );
  }
}
