#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>  // Adafruit DHT sensor library

// ========================= Configurações - variáveis editáveis =========================
const char* default_SSID = "Lifonix.IOT";
const char* default_PASSWORD = "lifonixforthefuture";
const char* default_BROKER_MQTT = "54.172.140.81";
const int   default_BROKER_PORT = 1981;

// ========================= Configurações - Dispositivo =========================
const char* default_TOPICO_SUBSCRIBE = "/TEF/device001/cmd";    
const char* default_TOPICO_PUBLISH_1 = "/TEF/device001/attrs";  
const char* default_TOPICO_PUBLISH_2 = "/TEF/device001/attrs/p";   // Luminosidade (%)
const char* default_TOPICO_PUBLISH_H = "/TEF/device001/attrs/h";   // Umidade do ar (%)
const char* default_TOPICO_PUBLISH_T = "/TEF/device001/attrs/t";   // Temperatura (°C)
const char* default_TOPICO_PUBLISH_SOIL = "/TEF/device001/attrs/soil"; // Umidade do solo (%)
const char* default_TOPICO_PUBLISH_AIR = "/TEF/device001/attrs/air";   // Qualidade do ar (AQI)
const char* default_ID_MQTT = "fiware_001";                     
const char* topicPrefix = "device001";                          
const char* localizacao = "São Paulo";

// ========================= Configurações DHT =========================
#define DHTPIN 4      
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

// ========================= Sensores analógicos extras =========================
const int LDR_PIN = 34;   // Luminosidade
const int SOIL_PIN = 35;  // Umidade do solo
const int AIR_PIN = 32;   // Qualidade do ar 

// ========================= Variáveis =========================
WiFiClient espClient;
PubSubClient MQTT(espClient);

// ========================= Protótipos =========================
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi();
void reconnectMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT();

void handleLuminosity();
void handleHumidity();
void handleTemperature();
void handleSoilHumidity();
void handleAirQuality();

// ========================= Inicializações =========================
void initSerial() { Serial.begin(115200); }

void initWiFi() {
  delay(10);
  Serial.println("------ Conexao WI-FI ------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(default_SSID);
  reconectWiFi();
}

void initMQTT() {
  MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

// ========================= Setup =========================
void setup() {
  initSerial();
  initWiFi();
  initMQTT();
  dht.begin(); 
  delay(2000);
  Serial.println("✅ Sistema iniciado e pronto para enviar dados MQTT!");
}

// ========================= Loop =========================
void loop() {
  VerificaConexoesWiFIEMQTT();

  handleLuminosity();
  handleHumidity();
  handleTemperature();
  handleSoilHumidity();
  handleAirQuality();

  MQTT.loop();
  delay(3000); // intervalo entre leituras
}

// ========================= Wi-Fi =========================
void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(default_SSID, default_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado com sucesso!");
  Serial.print("IP Local: ");
  Serial.println(WiFi.localIP());
}

// ========================= Callback MQTT =========================
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("📩 Mensagem recebida no tópico ");
  Serial.print(topic);
  Serial.print(": ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ========================= Conexões =========================
void VerificaConexoesWiFIEMQTT() {
  if (!MQTT.connected()) reconnectMQTT();
  reconectWiFi();
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("🔄 Conectando ao Broker MQTT: ");
    Serial.println(default_BROKER_MQTT);
    if (MQTT.connect(default_ID_MQTT)) {
      Serial.println("✅ Conectado ao broker!");
      MQTT.subscribe(default_TOPICO_SUBSCRIBE);
    } else {
      Serial.println("❌ Falha, tentando novamente em 3s...");
      delay(3000);
    }
  }
}

// ========================= Sensores =========================

// Luminosidade (%)
void handleLuminosity() {
  int sensorValue = analogRead(LDR_PIN);
  int luminosity = map(sensorValue, 0, 4095, 0, 100);
  Serial.print("💡 Luminosidade: ");
  Serial.print(luminosity);
  Serial.println(" %");

  String payload = "{\"local\":\"" + String(localizacao) + "\",\"luminosidade\":" + String(luminosity) + "}";
  MQTT.publish(default_TOPICO_PUBLISH_2, payload.c_str());
}

// Umidade do ar (%)
void handleHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) return;
  Serial.print("💧 Umidade do ar: ");
  Serial.print(h);
  Serial.println(" %");

  String payload = "{\"local\":\"" + String(localizacao) + "\",\"umidade\":" + String(h) + "}";
  MQTT.publish(default_TOPICO_PUBLISH_H, payload.c_str());
}

// Temperatura (°C)
void handleTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) return;
  Serial.print("🌡️ Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");

  String payload = "{\"local\":\"" + String(localizacao) + "\",\"temperatura\":" + String(t) + "}";
  MQTT.publish(default_TOPICO_PUBLISH_T, payload.c_str());
}

// Umidade do solo (%)
void handleSoilHumidity() {
  int soilValue = analogRead(SOIL_PIN);
  int soilPercent = map(soilValue, 4095, 0, 0, 100);
  Serial.print("🌱 Umidade do solo: ");
  Serial.print(soilPercent);
  Serial.println(" %");

  String payload = "{\"local\":\"" + String(localizacao) + "\",\"umidadeSolo\":" + String(soilPercent) + "}";
  MQTT.publish(default_TOPICO_PUBLISH_SOIL, payload.c_str());
}

// Qualidade do ar (AQI)
void handleAirQuality() {
  int mqValue = analogRead(AIR_PIN);
  int airQuality = map(mqValue, 0, 4095, 0, 500); 
  String airStatus;
  if (airQuality < 100) airStatus = "Boa";
  else if (airQuality < 200) airStatus = "Moderada";
  else if (airQuality < 300) airStatus = "Ruim";
  else airStatus = "Muito Ruim";

  Serial.print("🌫️ Qualidade do ar (simulada): AQI=");
  Serial.print(airQuality);
  Serial.print(" → ");
  Serial.println(airStatus);

  String payload = "{\"local\":\"" + String(localizacao) + "\",\"qualidadeAr\":" + String(airQuality) + ",\"status\":\"" + airStatus + "\"}";
  MQTT.publish(default_TOPICO_PUBLISH_AIR, payload.c_str());
}
