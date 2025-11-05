# Sistema de Monitoreo de Temperatura con IoT

## Descripción del Proyecto

Este proyecto consiste en el desarrollo de un sistema IoT para el monitoreo de temperatura, utilizando dos microcontroladores conectados mediante comunicación I2C y un sensor de temperatura LM35.
El sistema envía los datos a la plataforma **ThingSpeak**, donde se visualizan en tiempo real junto con el historial de valores, además de generar alertas cuando la temperatura supera los 30°C.

---

## Objetivos

* Diseñar un sistema funcional de medición de temperatura en tiempo real.
* Implementar comunicación entre dos microcontroladores usando I2C.
* Desplegar los datos de temperatura en un panel **ThingSpeak**.
* Generar alertas visuales mediante un LED cuando se supere el umbral de temperatura.
* Evaluar el rendimiento y confiabilidad del sistema mediante pruebas de funcionamiento.

---

## Requerimientos del Proyecto

**Herramientas y recursos utilizados:**

* Dos microcontroladores con comunicación I2C (2 Arduino UNO).
* Sensor de temperatura LM35.
* Un LED como actuador.
* Cuenta en ThingSpeak para visualización de datos.
* Conexión a Internet para el envío de información (ESP8266).
* Lenguaje de programación: C++ (Arduino IDE).

---

## Diagrama de Actividad

A continuación se muestra el flujo principal del sistema:

```plaintext
Inicio
   ↓
Leer sensor de temperatura
   ↓
Enviar datos al microcontrolador maestro
   ↓
Microcontrolador maestro publica datos en ThingSpeak
   ↓
¿Temperatura > 30°C?
   ├── Sí → Encender LED de alerta
   └── No → Apagar LED
   ↓
Esperar siguiente lectura
   ↓
Fin
```

---

## Arquitectura del Sistema

El sistema está compuesto por:

1. **Nodo Sensor (Esclavo I2C):**
   Lee la temperatura desde el sensor LM35/TMP36 y envía el valor al nodo maestro.

2. **Nodo Maestro (Arduino UNO conectado a ESP8266 para WiFi ):**
   Recibe la temperatura del esclavo, la muestra en el monitor serial y la envía a ThingSpeak para su almacenamiento y visualización.

3. **Plataforma ThingSpeak:**
   Muestra en tiempo real el valor actual de la temperatura, genera una gráfica histórica y permite analizar tendencias.

---

## Código Fuente

A continuación se incluyen los fragmentos de código utilizados para la implementación.

### Código del Nodo Esclavo (Sensor)

```cpp
#include <Wire.h>

#define SLAVE_ADDR 8     // Dirección I2C del esclavo
#define SENSOR_PIN A0    // Pin donde está el LM35

float temperature = 0;

void setup() {
  Wire.begin(SLAVE_ADDR);   // Iniciar como esclavo I2C
  Wire.onRequest(sendData); // Función que se llama cuando el maestro pide datos
  Serial.begin(9600);
  Serial.println("Peripheral listo...");
}

void loop() {
  int sensorValue = analogRead(SENSOR_PIN);
  temperature = (sensorValue * 5.0 * 100.0) / 1024.0;  // Conversión LM35 → °C
  Serial.print("Temperatura medida: ");
  Serial.print(temperature);
  Serial.println(" °C");
  delay(1000); // Medición cada 1 segundo
}

void sendData() {
  Wire.write((byte*)&temperature, sizeof(temperature)); // Enviar temperatura al maestro
}

```

### Código del Nodo Maestro (WiFi y Envío a ThingSpeak)

```cpp
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
  // Leer temperatura del esclavo
  Wire.requestFrom(SLAVE_ADDR, sizeof(float));
  if (Wire.available() == sizeof(float)) {
    Wire.readBytes((byte*)&temperatura, sizeof(temperatura));
    Serial.print("🌡 Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");

    //  Control del LED
    if (temperatura > 30.0) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Temperatura alta: LED encendido");
    } else {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Temperatura normal: LED apagado");
    }

    // Envío a ThingSpeak
    enviarThingSpeak(temperatura);
  }

  delay(15000);  // Espera 15 segundos entre envíos
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


```

---

## Resultados de Pruebas

Durante las pruebas se verificaron los siguientes aspectos:

* Comunicación I2C exitosa entre los dos microcontroladores.
* Lectura estable del sensor de temperatura con variaciones esperadas.
* Conectividad WiFi confiable del módulo ESP8266.
* Actualización de datos en ThingSpeak con intervalos regulares.
* Activación del LED cuando la temperatura superó los 30°C.
* Visualización correcta del valor actual y gráfico histórico en ThingSpeak.

Se adjuntan capturas del panel de **ThingSpeak** con los datos registrados:

<img width="1080" height="699" alt="image" src="https://github.com/user-attachments/assets/afed73cd-4a39-41a1-8e20-1e116e0390b4" />




## Contribución del Equipo

Este proyecto fue desarrollado por dos integrantes.
Las actividades se distribuyeron de la siguiente manera:

### Actividad 1: Construcción de la Wiki (Hector Diaz y Juan Pinilla)

* Documentación del proyecto.
* Elaboración del diagrama de actividades y estructura de la wiki.
* Redacción del informe técnico en formato README.

### Actividad 2: Desarrollo de Software y Hardware (Hector Diaz)

* Conexión de los componentes electrónicos.
* Programación de los microcontroladores.
* Pruebas de comunicación I2C y transmisión de datos a ThingSpeak.
* Configuración del canal y panel de visualización en ThingSpeak.

---

## Conclusiones

El sistema demostró un funcionamiento confiable para el monitoreo remoto de temperatura.
La comunicación I2C y el envío a ThingSpeak se realizaron de manera estable, permitiendo una integración completa entre hardware y software.
Este proyecto evidencia la aplicabilidad de soluciones IoT en el monitoreo ambiental mediante sensores de bajo costo y conectividad en la nube.

---

## Uso de inteligencia Artificial

Parte del borrador de la documentación fueron elaborados con apoyo de **ChatGPT (OpenAI, 2025)**. El contenido fue revisado, adaptado y complementado por el equipo. 
Asimismo, se le pidio ayuda a **ChatGPT (OpenAI, 2025)* para la implementacion del ESP8266 y su implementacion a ThingSpeak. El prompt fue "Ayudame a conectar el ESP8266 para enviar los datos a ThingSpeak ya que no me deja. Imprime el proceso para saber en que esta fallando"

---

## Referencias

* ThingSpeak IoT Analytics Platform. [https://thingspeak.com](https://thingspeak.com)
* Arduino Documentation. [https://www.arduino.cc/reference](https://www.arduino.cc/reference)
* Espressif ESP8266 AT Commands Set.
* Sensor LM35 Datasheet.
* https://youtu.be/R9t4XWNwYr0?si=XWSwCkWOmI1Bjft_
* https://youtu.be/S7pkknfh3uw?si=Edef57zItW0cEOQG
* https://youtu.be/rEBT9evGeFY?si=KPQWpx8lEG-oUTSZ
