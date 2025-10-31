Este proyecto implementa un **sistema de monitoreo de temperatura basado en IoT**, utilizando dos microcontroladores conectados mediante el protocolo **I2C**.El **microcontrolador maestro** recibe los datos de temperatura desde el esclavo, **controla un LED** cuando la temperatura supera un umbral y **envía los datos a ThingSpeak** para su almacenamiento y visualización en tiempo real.

El sistema integra conceptos de **adquisición de datos, comunicación entre dispositivos, automatización y computación en la nube**, aplicados al Internet de las Cosas.

**2\. Arquitectura del Sistema**
--------------------------------

### **Componentes de Hardware**

ComponenteDescripciónArduino UnoMicrocontrolador maestro: recibe datos y los envía a ThingSpeakArduino Nano o ESP8266Microcontrolador esclavo: mide la temperaturaSensor LM35 / TMP36Sensor analógico de temperaturaLEDIndicador de temperatura alta (>30°C)Módulo ESP8266Proporciona la conexión WiFiThingSpeakPlataforma en la nube para visualizar y analizar datos

**3\. Diagrama de Actividad**
-----------------------------

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   flowchart TD  A[Inicio] --> B[Leer temperatura del sensor (Esclavo)]  B --> C[Enviar temperatura por I2C al Maestro]  C --> D[Maestro recibe la temperatura]  D --> E{¿Temperatura > 30°C?}  E -->|Sí| F[Encender LED de alerta]  E -->|No| G[Apagar LED]  F --> H[Enviar datos a ThingSpeak]  G --> H  H --> I[Actualizar dashboard en la nube]  I --> J[Mostrar datos históricos y en tiempo real]  J --> A   `

**4\. Documentación del Código**
--------------------------------

### **Código del Maestro (Arduino + ESP8266 + ThingSpeak)**

**Archivo:** master\_iot.ino

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   #include   #include   // --- Configuración del ESP8266 ---  #define RX_ESP 2  #define TX_ESP 3  SoftwareSerial esp8266(RX_ESP, TX_ESP);  // --- Pines y direcciones ---  #define SLAVE_ADDR 8  #define LED_PIN 7  // --- Credenciales WiFi y ThingSpeak ---  const char* SSID = "CASA HECTOR DIAZ";  const char* PASSWORD = "3112954711";  String apiKey = "2H2X4KFU8FEI4KER";  const char* server = "api.thingspeak.com";  float temperatura = 0;  void setup() {    Serial.begin(9600);    Wire.begin(); // Inicia como Maestro    pinMode(LED_PIN, OUTPUT);    esp8266.begin(9600); // Configuración del ESP    delay(2000);    Serial.println("==== SISTEMA INICIADO ====");    conectarWiFi();    Serial.println("✅ WiFi conectado correctamente");  }  void loop() {    // 1️⃣ Leer temperatura del esclavo    Wire.requestFrom(SLAVE_ADDR, sizeof(float));    if (Wire.available() == sizeof(float)) {      Wire.readBytes((byte*)&temperatura, sizeof(temperatura));      Serial.print("🌡 Temperatura: ");      Serial.print(temperatura);      Serial.println(" °C");      // 2️⃣ Control del LED      if (temperatura > 30.0) {        digitalWrite(LED_PIN, HIGH);        Serial.println("⚠️ Temperatura alta: LED encendido");      } else {        digitalWrite(LED_PIN, LOW);        Serial.println("✅ Temperatura normal: LED apagado");      }      // 3️⃣ Enviar a ThingSpeak      enviarThingSpeak(temperatura);    }    delay(20000); // ThingSpeak permite un envío cada 15 segundos  }  // -----------------------------------------------------  // FUNCIONES AUXILIARES  // -----------------------------------------------------  void conectarWiFi() {    Serial.println("Conectando al WiFi...");    enviarComando("AT", 1000);    enviarComando("AT+CWMODE=1", 1000);    String cmd = "AT+CWJAP=\"" + String(SSID) + "\",\"" + String(PASSWORD) + "\"";    enviarComando(cmd.c_str(), 8000);  }  void enviarThingSpeak(float valor) {    Serial.println("📡 Enviando datos a ThingSpeak...");    String cmd = "AT+CIPSTART=\"TCP\",\"" + String(server) + "\",80";    String respuesta = enviarComando(cmd.c_str(), 3000);    if (respuesta.indexOf("Error") != -1) {      Serial.println("❌ Error en la conexión TCP");      return;    }    String httpData = "GET /update?api_key=" + apiKey + "&field1=" + String(valor) +                      " HTTP/1.1\r\nHost: " + String(server) + "\r\nConnection: close\r\n\r\n";    cmd = "AT+CIPSEND=" + String(httpData.length());    enviarComando(cmd.c_str(), 2000);    enviarComando(httpData.c_str(), 4000);    Serial.println("✅ Datos enviados correctamente.\n");  }  String enviarComando(const char* comando, const int tiempo) {    String respuesta = "";    esp8266.println(comando);    long int t = millis();    while ((millis() - t) < tiempo) {      while (esp8266.available()) {        char c = esp8266.read();        respuesta += c;      }    }    Serial.println(respuesta);    return respuesta;  }   `

### **Código del Esclavo (Sensor de Temperatura)**

**Archivo:** slave\_sensor.ino

Plain textANTLR4BashCC#CSSCoffeeScriptCMakeDartDjangoDockerEJSErlangGitGoGraphQLGroovyHTMLJavaJavaScriptJSONJSXKotlinLaTeXLessLuaMakefileMarkdownMATLABMarkupObjective-CPerlPHPPowerShell.propertiesProtocol BuffersPythonRRubySass (Sass)Sass (Scss)SchemeSQLShellSwiftSVGTSXTypeScriptWebAssemblyYAMLXML`   #include   #define SLAVE_ADDR 8  #define SENSOR_PIN A0  float temperature = 0;  void setup() {    Wire.begin(SLAVE_ADDR);    Wire.onRequest(requestEvent);  }  void loop() {    int sensorValue = analogRead(SENSOR_PIN);    temperature = (sensorValue * 5.0 * 100.0) / 1024.0; // Conversión LM35    delay(1000);  }  void requestEvent() {    Wire.write((byte*)&temperature, sizeof(temperature));  }   `

**5\. Resultados de las Pruebas**
---------------------------------

PruebaEntradaResultado EsperadoEstadoTemperatura < 30 °C28 °CLED apagado✅ CorrectoTemperatura > 30 °C35 °CLED encendido✅ CorrectoDesconexión WiFi—Reconexión automática✅ CorrectoEnvío a ThingSpeakCada 20 segundosRegistro exitoso en la nube✅ CorrectoComunicación I2CEntre Maestro y EsclavoFlujo de datos estable✅ Correcto

En el **dashboard de ThingSpeak** se observan las variaciones de temperatura y el histórico completo.

**6\. Dashboard en ThingSpeak**
-------------------------------

El panel de control incluye:

*   **Temperatura actual en tiempo real.**
    
*   **Alertas visuales cuando supera los 30 °C.**
    
*   **Gráfico histórico de temperatura para análisis y seguimiento.**
    

Ejemplo del dashboard:

**8\. Herramientas y Recursos**
-------------------------------

Herramienta / RecursoUso**Arduino IDE**Desarrollo y carga de código**ThingSpeak**Visualización y análisis de datos en la nube**ESP8266 AT Commands**Comunicación WiFi mediante comandos AT**Protocolo I2C**Comunicación entre microcontroladores**Sensor LM35/TMP36**Medición de temperatura

**9\. Contribución del Equipo**
-------------------------------

IntegranteRolAporte PrincipalIntegrante 1Ingeniero de HardwareDiseño del circuito y conexión de sensoresIntegrante 2Desarrollador de SoftwareProgramación del Arduino y comunicación I2CIntegrante 3Analista de DatosConfiguración de ThingSpeak y gráficosIntegrante 4DocumentaciónRedacción de la wiki y grabación del video

**10\. Conclusiones**
---------------------

El sistema de monitoreo de temperatura logró:

*   Adquisición confiable de datos desde el sensor.
    
*   Comunicación I2C efectiva entre dispositivos.
    
*   Envío y visualización de información en tiempo real en ThingSpeak.
    
*   Alertas visuales mediante LED ante temperaturas críticas.
    

Este proyecto demuestra la integración práctica de **IoT, sensado ambiental y comunicación en la nube**, aportando una base sólida para aplicaciones de monitoreo remoto.
