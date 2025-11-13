#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========================= Configurações de Rede e Broker =========================
const char* WIFI_SSID = "Lifonix.IOT";
const char* WIFI_PASSWORD = "lifonixforthefuture";
const char* BROKER_MQTT = "broker.hivemq.com";   // Broker público para teste
const int   BROKER_PORT = 1883;
const char* MQTT_ID = "lifonix_workwell_esp32";

// ========================= Tópicos MQTT =========================
const char* TOPICO_PUBLISH_STATUS = "/lifonix/workwell/status";
const char* TOPICO_PUBLISH_ENV = "/lifonix/workwell/env";
const char* TOPICO_SUBSCRIBE = "/lifonix/workwell/cmd";

// ========================= Configurações de Sensores =========================
#define DHTPIN 4
#define DHTTYPE DHT22
#define LDR_PIN 34
#define NOISE_PIN 35      // Potenciômetro simulando ruído
#define BUTTON_PIN 19
#define LED_R 16
#define LED_G 17
#define LED_B 18

DHT dht(DHTPIN, DHTTYPE);

// ========================= Objetos Globais =========================
WiFiClient espClient;
PubSubClient MQTT(espClient);

// ========================= Funções de Inicialização =========================
void initSerial() {
  Serial.begin(115200);
  Serial.println("\n=== Iniciando Lifonix WorkWell ===");
}

void initWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n📩 Comando recebido [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ========================= Reconexão Automática =========================
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("🔄 Conectando ao Broker MQTT...");
    if (MQTT.connect(MQTT_ID)) {
      Serial.println(" conectado!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.print("❌ Falhou, rc=");
      Serial.print(MQTT.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// ========================= LED RGB =========================
void setColor(int r, int g, int b) {
  ledcWrite(0, r); // canal 0
  ledcWrite(1, g); // canal 1
  ledcWrite(2, b); // canal 2
}

// ========================= Setup =========================
void setup() {
  initSerial();
  initWiFi();
  initMQTT();
  dht.begin();

  pinMode(LDR_PIN, INPUT);
  pinMode(NOISE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Configurar canais PWM para LED RGB
  ledcSetup(0, 5000, 8);
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  ledcAttachPin(LED_R, 0);
  ledcAttachPin(LED_G, 1);
  ledcAttachPin(LED_B, 2);

  Serial.println("✅ Sistema Lifonix WorkWell iniciado!");
}

// ========================= Loop Principal =========================
void loop() {
  if (!MQTT.connected()) reconnectMQTT();
  MQTT.loop();

  // ======== Leitura dos sensores ========
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int light = analogRead(LDR_PIN);
  int noise = analogRead(NOISE_PIN);
  bool pausePressed = digitalRead(BUTTON_PIN) == LOW;

  if (isnan(temp) || isnan(hum)) {
    Serial.println("⚠️ Erro ao ler o DHT22");
    delay(2000);
    return;
  }

  // ======== Normalização de valores ========
  int lightPercent = map(light, 0, 4095, 0, 100);
  int noiseLevel = map(noise, 0, 4095, 0, 100);

  // ======== Lógica de decisão automática ========
  String status;
  if (temp > 28 || noiseLevel > 70 || lightPercent < 30) {
    status = "ALERTA";
    setColor(255, 200, 0);  // LED amarelo
  } else if (pausePressed) {
    status = "PAUSA";
    setColor(0, 0, 255);    // LED azul
  } else {
    status = "IDEAL";
    setColor(0, 255, 0);    // LED verde
  }

  // ======== Publicação MQTT ========
  String envPayload = "{\"temperatura\":" + String(temp) +
                      ",\"umidade\":" + String(hum) +
                      ",\"luminosidade\":" + String(lightPercent) +
                      ",\"ruido\":" + String(noiseLevel) + "}";
  MQTT.publish(TOPICO_PUBLISH_ENV, envPayload.c_str());

  String statusPayload = "{\"status\":\"" + status + "\"}";
  MQTT.publish(TOPICO_PUBLISH_STATUS, statusPayload.c_str());

  Serial.println("\n📡 Dados enviados:");
  Serial.println(envPayload);
  Serial.println(statusPayload);

  delay(3000); // intervalo de leitura
}
