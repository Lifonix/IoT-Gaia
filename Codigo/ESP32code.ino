#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========================= Configurações =========================
const char* default_SSID        = "Wokwi-GUEST";
const char* default_PASSWORD    = "";
const char* default_BROKER_MQTT = "44.223.43.74";
const int   default_BROKER_PORT = 1883;
const int   default_D4          = 2;

const char* default_TOPICO_SUBSCRIBE = "/lifonix/workwell/cmd";
const char* default_TOPICO_ENV       = "/lifonix/workwell/env";     // ENV JSON
const char* default_TOPICO_STATUS    = "/lifonix/workwell/status";  // online/offline

const char* default_TOPICO_PUBLISH_1 = "/lifonix/workwell/attrs";
const char* default_TOPICO_PUBLISH_2 = "/lifonix/workwell/attrs/p"; 
const char* default_TOPICO_PUBLISH_H = "/lifonix/workwell/attrs/h"; 
const char* default_TOPICO_PUBLISH_T = "/lifonix/workwell/attrs/t"; 

const char* default_ID_MQTT = "fiware_001";
const char* topicPrefix     = "workwell";

// ========================= Pinos =========================
#define DHTPIN  4
#define DHTTYPE DHT22
int pinLuminosity = 34;

DHT dht(DHTPIN, DHTTYPE);

// ========================= Variáveis =========================
WiFiClient espClient;
PubSubClient MQTT(espClient);

char EstadoSaida = '0';

char* SSID        = const_cast<char*>(default_SSID);
char* PASSWORD    = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int   BROKER_PORT = default_BROKER_PORT;

// ========================= Funções =========================
void initSerial() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== workwell iniciando ===");
}

// WiFi com timeout
bool conectaWiFiComTimeout(unsigned long timeoutMs) {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < timeoutMs) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi conectado! IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(default_D4, LOW);
    return true;
  }

  Serial.println("Falha ao conectar no WiFi!");
  return false;
}

void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  bool ok = conectaWiFiComTimeout(8000);
  if (!ok) Serial.println("Ainda desconectado. Tentará novamente...");
}

void initWiFi() {
  conectaWiFiComTimeout(10000);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("MQTT MSG [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  String onCmd  = String(topicPrefix) + "@on|";
  String offCmd = String(topicPrefix) + "@off|";

  if (msg == onCmd) {
    digitalWrite(default_D4, HIGH);
    EstadoSaida = '1';
  }
  if (msg == offCmd) {
    digitalWrite(default_D4, LOW);
    EstadoSaida = '0';
  }
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;

  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker...");
    if (MQTT.connect(default_ID_MQTT)) {
      Serial.println(" OK!");
      MQTT.subscribe(default_TOPICO_SUBSCRIBE);
    } else {
      Serial.print("Falhou, rc=");
      Serial.println(MQTT.state());
      delay(2000);
    }
  }
}

void VerificaConexoesWiFIEMQTT() {
  reconectWiFi();
  if (WiFi.status() == WL_CONNECTED && !MQTT.connected()) {
    reconnectMQTT();
  }
}

// envia estado do relé
void EnviaEstadoOutputMQTT() {
  if (EstadoSaida == '1') MQTT.publish(default_TOPICO_PUBLISH_1, "s|on");
  if (EstadoSaida == '0') MQTT.publish(default_TOPICO_PUBLISH_1, "s|off");
}

void InitOutput() {
  pinMode(default_D4, OUTPUT);
  digitalWrite(default_D4, HIGH);
  delay(200);
  digitalWrite(default_D4, LOW);
  EstadoSaida = '0';
}

// ========================= SENSORES =========================

void handleLuminosity() {
  static unsigned long last = 0;           
  if (millis() - last < 1000) return;

  int raw = analogRead(pinLuminosity);
  int lum = map(raw, 0, 4095, 0, 100);

  char payload[8];
  itoa(lum, payload, 10);
  MQTT.publish(default_TOPICO_PUBLISH_2, payload);

  last = millis();
}

void handleHumidity() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float h = dht.readHumidity();
  if (!isnan(h)) {
    char payload[16];
    dtostrf(h, 0, 1, payload);
    MQTT.publish(default_TOPICO_PUBLISH_H, payload);
  }

  last = millis();
}

void handleTemperature() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float t = dht.readTemperature();
  if (!isnan(t)) {
    char payload[16];
    dtostrf(t, 0, 1, payload);
    MQTT.publish(default_TOPICO_PUBLISH_T, payload);
  }

  last = millis();
}

// ========================= ENV JSON =========================
void sendEnv() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int raw = analogRead(pinLuminosity);
  int lum = map(raw, 0, 4095, 0, 100);

  if (isnan(h) || isnan(t)) return;

  char json[128];
  snprintf(json, sizeof(json),
           "{\"temp\":%.1f,\"umid\":%.1f,\"lum\":%d}",
           t, h, lum);

  MQTT.publish(default_TOPICO_ENV, json);

  Serial.print("ENV JSON: ");
  Serial.println(json);

  last = millis();
}

// ========================= STATUS =========================
void sendStatus() {
  static unsigned long last = 0;
  if (millis() - last < 5000) return;

  if (WiFi.status() == WL_CONNECTED && MQTT.connected()) {
    MQTT.publish(default_TOPICO_STATUS, "online");
  } else {
    MQTT.publish(default_TOPICO_STATUS, "offline");
  }

  last = millis();
}

// ========================= SETUP =========================
void setup() {
  InitOutput();
  initSerial();
  initWiFi();
  initMQTT();
  dht.begin();
  delay(2000);
}

// ========================= LOOP =========================
void loop() {
  VerificaConexoesWiFIEMQTT();
  EnviaEstadoOutputMQTT();

  handleLuminosity();
  handleHumidity();
  handleTemperature();

  sendEnv();     // envia JSON ENV
  sendStatus();  // envia status simples

  MQTT.loop();
}
