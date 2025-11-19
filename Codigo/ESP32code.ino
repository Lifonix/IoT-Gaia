#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========================= Configurações =========================
// Credenciais e parâmetros padrão de WiFi e MQTT
const char* default_SSID        = "Wokwi-GUEST";
const char* default_PASSWORD    = "";
const char* default_BROKER_MQTT = "44.223.43.74";
const int   default_BROKER_PORT = 1883;
const int   default_D4          = 2;              // LED / saída digital

// Tópicos MQTT usados pelo dispositivo
const char* default_TOPICO_SUBSCRIBE = "/lifonix/workwell/cmd";      // comandos recebidos
const char* default_TOPICO_ENV       = "/lifonix/workwell/env";      // JSON com ambiente
const char* default_TOPICO_STATUS    = "/lifonix/workwell/status";   // online/offline

const char* default_TOPICO_PUBLISH_1 = "/lifonix/workwell/attrs";    // estado de saída
const char* default_TOPICO_PUBLISH_2 = "/lifonix/workwell/attrs/p";  // luminosidade (%)
const char* default_TOPICO_PUBLISH_H = "/lifonix/workwell/attrs/h";  // umidade (DHT)
const char* default_TOPICO_PUBLISH_T = "/lifonix/workwell/attrs/t";  // temperatura (DHT)

const char* default_ID_MQTT = "fiware_001";       // ID do cliente MQTT
const char* topicPrefix     = "workwell";         // prefixo para comandos

// ========================= Pinos =========================
#define DHTPIN  4           // pino do DHT22
#define DHTTYPE DHT22
int pinLuminosity = 34;     // entrada analógica para sensor de luz

DHT dht(DHTPIN, DHTTYPE);

// ========================= Variáveis =========================
// Objetos de rede e MQTT
WiFiClient espClient;
PubSubClient MQTT(espClient);

char EstadoSaida = '0';     // guarda o estado da saída (LED/relé)

// Permite sobrescrever as configs em tempo de execução, se necessário
char* SSID        = const_cast<char*>(default_SSID);
char* PASSWORD    = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int   BROKER_PORT = default_BROKER_PORT;

// ========================= Funções =========================
// Inicializa a serial
void initSerial() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== workwell iniciando ===");
}

// Tenta conectar ao WiFi com timeout configurável
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

  // Se conectou com sucesso
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi conectado! IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(default_D4, LOW);   // garante saída em nível baixo
    return true;
  }

  // Falha na conexão
  Serial.println("Falha ao conectar no WiFi!");
  return false;
}

// Reforça reconexão ao WiFi se cair
void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  bool ok = conectaWiFiComTimeout(8000);
  if (!ok) Serial.println("Ainda desconectado. Tentará novamente...");
}

// Chamada inicial para conectar ao WiFi
void initWiFi() {
  conectaWiFiComTimeout(10000);
}

// Callback disparado quando chega uma mensagem MQTT em tópico inscrito
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("MQTT MSG [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  // Formato esperado de comando: workwell@on|  ou workwell@off|
  String onCmd  = String(topicPrefix) + "@on|";
  String offCmd = String(topicPrefix) + "@off|";

  if (msg == onCmd) {
    digitalWrite(default_D4, HIGH);  // liga saída
    EstadoSaida = '1';
  }
  if (msg == offCmd) {
    digitalWrite(default_D4, LOW);   // desliga saída
    EstadoSaida = '0';
  }
}

// Configura parâmetros do cliente MQTT (broker e callback)
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

// Tenta reconectar ao broker MQTT caso desconecte
void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;

  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker...");
    if (MQTT.connect(default_ID_MQTT)) {
      Serial.println(" OK!");
      // Inscreve no tópico de comandos ao conectar
      MQTT.subscribe(default_TOPICO_SUBSCRIBE);
    } else {
      Serial.print("Falhou, rc=");
      Serial.println(MQTT.state());
      delay(2000); // espera antes de tentar de novo
    }
  }
}

// Verifica e garante conexões WiFi e MQTT ativas
void VerificaConexoesWiFIEMQTT() {
  reconectWiFi();
  if (WiFi.status() == WL_CONNECTED && !MQTT.connected()) {
    reconnectMQTT();
  }
}

// Publica o estado atual da saída (on/off) em um tópico de atributos
void EnviaEstadoOutputMQTT() {
  if (EstadoSaida == '1') MQTT.publish(default_TOPICO_PUBLISH_1, "s|on");
  if (EstadoSaida == '0') MQTT.publish(default_TOPICO_PUBLISH_1, "s|off");
}

// Inicializa o pino de saída (LED/relé) com um pequeno flash
void InitOutput() {
  pinMode(default_D4, OUTPUT);
  digitalWrite(default_D4, HIGH);
  delay(200);
  digitalWrite(default_D4, LOW);
  EstadoSaida = '0';
}

// ========================= SENSORES =========================

// Lê luminosidade a cada 1s e publica em porcentagem
void handleLuminosity() {
  static unsigned long last = 0;           
  if (millis() - last < 1000) return;

  int raw = analogRead(pinLuminosity);        // leitura bruta 0–4095
  int lum = map(raw, 0, 4095, 0, 100);       // converte para 0–100 %

  char payload[8];
  itoa(lum, payload, 10);                    // converte inteiro para string
  MQTT.publish(default_TOPICO_PUBLISH_2, payload);

  last = millis();
}

// Lê umidade do DHT22 a cada 2s e publica
void handleHumidity() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float h = dht.readHumidity();
  if (!isnan(h)) {                           // só envia se leitura for válida
    char payload[16];
    dtostrf(h, 0, 1, payload);              // formata com 1 casa decimal
    MQTT.publish(default_TOPICO_PUBLISH_H, payload);
  }

  last = millis();
}

// Lê temperatura do DHT22 a cada 2s e publica
void handleTemperature() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float t = dht.readTemperature();
  if (!isnan(t)) {
    char payload[16];
    dtostrf(t, 0, 1, payload);              // formata com 1 casa decimal
    MQTT.publish(default_TOPICO_PUBLISH_T, payload);
  }

  last = millis();
}

// ========================= ENV JSON =========================
// Publica um JSON consolidado com temp, umidade e luminosidade
void sendEnv() {
  static unsigned long last = 0;
  if (millis() - last < 2000) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int raw = analogRead(pinLuminosity);
  int lum = map(raw, 0, 4095, 0, 100);

  if (isnan(h) || isnan(t)) return;         // evita enviar se DHT falhar

  char json[128];
  snprintf(json, sizeof(json),
           "{\"temp\":%.1f,\"umid\":%.1f,\"lum\":%d}",
           t, h, lum);                      // monta JSON

  MQTT.publish(default_TOPICO_ENV, json);   // envia para tópico ENV

  Serial.print("ENV JSON: ");
  Serial.println(json);

  last = millis();
}

// ========================= STATUS =========================
// Envia status simples (online/offline) a cada 5s
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
  InitOutput();   // prepara saída
  initSerial();   // inicia serial
  initWiFi();     // conecta ao WiFi
  initMQTT();     // configura MQTT
  dht.begin();    // inicia sensor DHT
  delay(2000);
}

// ========================= LOOP =========================
void loop() {
  VerificaConexoesWiFIEMQTT();  // garante WiFi + MQTT conectados
  EnviaEstadoOutputMQTT();      // publica estado atual da saída

  // Lê e envia dados individuais dos sensores
  handleLuminosity();
  handleHumidity();
  handleTemperature();

  // Envia pacote consolidado em JSON e status do dispositivo
  sendEnv();     
  sendStatus();  

  MQTT.loop();   // processa fila MQTT (envio/recebimento)
}
