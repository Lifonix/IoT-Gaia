#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========================= Configurações =========================
const char* default_SSID        = "Wokwi-GUEST";
const char* default_PASSWORD    = "";
const char* default_BROKER_MQTT = "44.223.43.74";
const int   default_BROKER_PORT = 1883;
const int   default_D4          = 2;

const char* default_TOPICO_SUBSCRIBE = "/TEF/device001/cmd";
const char* default_TOPICO_PUBLISH_1 = "/TEF/device001/attrs";
const char* default_TOPICO_PUBLISH_2 = "/TEF/device001/attrs/p"; // Luminosidade / som (%)
const char* default_TOPICO_PUBLISH_H = "/TEF/device001/attrs/h"; // Umidade
const char* default_TOPICO_PUBLISH_T = "/TEF/device001/attrs/t"; // Temperatura

const char* default_ID_MQTT = "fiware_001";
const char* topicPrefix     = "device001";

// ========================= Pinos configuráveis =========================
// DHT22 no GPIO4
#define DHTPIN  4
#define DHTTYPE DHT22

// Sensor analógico no GPIO34
int pinLuminosity = 34;

DHT dht(DHTPIN, DHTTYPE);

// ========================= Variáveis =========================
char* SSID        = const_cast<char*>(default_SSID);
char* PASSWORD    = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int   BROKER_PORT = default_BROKER_PORT;

char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_H = const_cast<char*>(default_TOPICO_PUBLISH_H);
char* TOPICO_PUBLISH_T = const_cast<char*>(default_TOPICO_PUBLISH_T);

char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int   D4      = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

// ========================= Funções =========================
void initSerial() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("=== device001 iniciando ===");
}

// Tenta conectar ao WiFi com timeout e mensagens claras
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

  int status = WiFi.status();
  Serial.print("Status WiFi apos tentativa: ");
  Serial.println(status); // 3 = WL_CONNECTED

  if (status == WL_CONNECTED) {
    Serial.print("WiFi CONECTADO. IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(D4, LOW);
    return true;
  } else {
    Serial.println("NAO conseguiu conectar ao WiFi (status diferente de 3).");
    return false;
  }
}

void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  // tenta por 10 segundos
  bool ok = conectaWiFiComTimeout(10000);
  if (!ok) {
    Serial.println("WiFi ainda desconectado. Vai tentar de novo depois...");
  }
}

void initWiFi() {
  Serial.println("Iniciando WiFi...");
  conectaWiFiComTimeout(10000); // primeira tentativa
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("MQTT MSG [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  String onTopic  = String(topicPrefix) + "@on|";
  String offTopic = String(topicPrefix) + "@off|";

  if (msg.equals(onTopic)) {
    digitalWrite(D4, HIGH);
    EstadoSaida = '1';
    Serial.println("Saida D4: ON");
  }
  if (msg.equals(offTopic)) {
    digitalWrite(D4, LOW);
    EstadoSaida = '0';
    Serial.println("Saida D4: OFF");
  }
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    // sem WiFi, nem tenta MQTT
    return;
  }

  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (MQTT.connect(ID_MQTT)) {
      Serial.println(" conectado!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
      Serial.print("Inscrito em: ");
      Serial.println(TOPICO_SUBSCRIBE);
    } else {
      Serial.print("falha, rc=");
      Serial.print(MQTT.state());
      Serial.println(" - tentando em 2s");
      delay(2000);
    }
  }
}

void VerificaConexoesWiFIEMQTT() {
  // primeiro garante WiFi
  reconectWiFi();

  // so tenta MQTT se WiFi estiver conectado
  if (WiFi.status() == WL_CONNECTED && !MQTT.connected()) {
    reconnectMQTT();
  }
}

void EnviaEstadoOutputMQTT() {
  if (EstadoSaida == '1') MQTT.publish(TOPICO_PUBLISH_1, "s|on");
  if (EstadoSaida == '0') MQTT.publish(TOPICO_PUBLISH_1, "s|off");
}

void InitOutput() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  boolean toggle = false;

  for (int i = 0; i <= 10; i++) {
    toggle = !toggle;
    digitalWrite(D4, toggle);
    delay(200);
  }

  digitalWrite(D4, LOW);
  EstadoSaida = '0';
}

// ========================= Leituras dos Sensores =========================

// Sensor analógico (%)
void handleLuminosity() {
  static unsigned long last = 0;           // guarda o último momento em que leu
  unsigned long agora = millis();

  // só lê e envia a cada 1000 ms (1 segundo)
  if (agora - last < 1000) {
    return;
  }

  int value = analogRead(pinLuminosity);          // leitura 0–4095
  int luminosity = map(value, 0, 4095, 0, 100);   // 0–100%

  Serial.print("Luminosidade raw: ");
  Serial.print(value);
  Serial.print("  |  Luminosidade (%): ");
  Serial.println(luminosity);

  char payload[8];
  itoa(luminosity, payload, 10);
  MQTT.publish(TOPICO_PUBLISH_2, payload);

  last = agora;  // atualiza o tempo da última leitura
}

// Umidade (DHT22)
void handleHumidity() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float h = dht.readHumidity();
  if (!isnan(h)) {
    Serial.print("Umidade (%): ");
    Serial.println(h);

    char payload[16];
    dtostrf(h, 0, 1, payload);
    MQTT.publish(TOPICO_PUBLISH_H, payload);
  } else {
    Serial.println("Falha ao ler umidade do DHT22");
  }

  last = millis();
}

// Temperatura (DHT22)
void handleTemperature() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float t = dht.readTemperature();
  if (!isnan(t)) {
    Serial.print("Temperatura (C): ");
    Serial.println(t);

    char payload[16];
    dtostrf(t, 0, 1, payload);
    MQTT.publish(TOPICO_PUBLISH_T, payload);
  } else {
    Serial.println("Falha ao ler temperatura do DHT22");
  }

  last = millis();
}

// ========================= Setup =========================
void setup() {
  InitOutput();
  initSerial();
  initWiFi();   // tenta conectar ao WiFi
  initMQTT();
  dht.begin();

  delay(2000);
  MQTT.publish(TOPICO_PUBLISH_1, "s|on");
}

// ========================= Loop =========================
void loop() {
  VerificaConexoesWiFIEMQTT();
  EnviaEstadoOutputMQTT();

  handleLuminosity();
  handleHumidity();
  handleTemperature();

  MQTT.loop();
}
