#include <Wire.h>
#include <SoftwareSerial.h>

// --- Pines ESP8266 ---
#define RX_ESP 2   // Arduino RX (recibe del TX del ESP)
#define TX_ESP 3   // Arduino TX (envía al RX del ESP)
SoftwareSerial esp8266(RX_ESP, TX_ESP);

// --- Pines y direcciones ---
#define SLAVE_ADDR 8
#define LED_PIN 7

// --- Credenciales WiFi y ThingSpeak ---
const char* SSID = "S20+";             // CAMBIAR ESTO
const char* PASSWORD = "12309876";     // CAMBIAR ESTO
String apiKey = "2H2X4KFU8FEI4KER";            // ESTO ES DE THINGSPEAK
const char* server = "api.thingspeak.com";

// --- Variables ---
float temperatura = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();                 // Inicia como Maestro I2C
  pinMode(LED_PIN, OUTPUT);
  
  esp8266.begin(9600);         
  delay(2000);
  
  Serial.println("==== INICIANDO SISTEMA ====");
  conectarWiFi();
  Serial.println("WiFi conectado correctamente ");
}

void loop() {
  // 1️ Leer temperatura del esclavo
  Wire.requestFrom(SLAVE_ADDR, sizeof(float));
  if (Wire.available() == sizeof(float)) {
    Wire.readBytes((byte*)&temperatura, sizeof(temperatura));
    Serial.print("🌡 Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");

    // 2️ Control del LED
    if (temperatura > 30.0) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Temperatura alta: LED encendido");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Temperatura normal: LED apagado");
    }

    // 3️ Envío a ThingSpeak
    enviarThingSpeak(temperatura);
  }

  delay(15000);  // Espera 20 segundos entre envíos (ThingSpeak requiere mínimo 15s)
}

// -----------------------------------------------------
// FUNCIONES AUXILIARES (AYUDADO POR CHATGPT PARA VERIFICAR CONEXION CON ESP8266 Y THINGSPEAK)
// -----------------------------------------------------

void conectarWiFi() {
  Serial.println("Conectando al WiFi...");
  enviarComando("AT", 1000);
  enviarComando("AT+CWMODE=1", 1000);
  
  String cmd = "AT+CWJAP=\"" + String(SSID) + "\",\"" + String(PASSWORD) + "\"";
  enviarComando(cmd.c_str(), 8000);
}

void enviarThingSpeak(float valor) {
  Serial.println(" Enviando datos a ThingSpeak...");

  String cmd = "AT+CIPSTART=\"TCP\",\"" + String(server) + "\",80";
  String respuesta = enviarComando(cmd.c_str(), 3000);
  if (respuesta.indexOf("Error") != -1) {
    Serial.println("Error en la conexión TCP");
    return;
  }

  String httpData = "GET /update?api_key=" + apiKey + "&field1=" + String(valor) +
                    " HTTP/1.1\r\nHost: " + String(server) + "\r\nConnection: close\r\n\r\n";

  cmd = "AT+CIPSEND=" + String(httpData.length());
  enviarComando(cmd.c_str(), 2000);
  enviarComando(httpData.c_str(), 4000);
  Serial.println("Datos enviados correctamente a ThingSpeak.\n");
}

String enviarComando(const char* comando, const int tiempo) {
  String respuesta = "";
  esp8266.println(comando);
  long int t = millis();
  while ((millis() - t) < tiempo) {
    while (esp8266.available()) {
      char c = esp8266.read();
      respuesta += c;
    }
  }
  Serial.println(respuesta);
  return respuesta;
}

