# Sistema de Monitoreo de Temperatura con IoT

## Descripción del Proyecto

Este proyecto consiste en el desarrollo de un sistema IoT para el monitoreo de temperatura, utilizando dos microcontroladores conectados mediante comunicación I2C y un sensor de temperatura LM35 (o TMP36).
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

* Dos microcontroladores con comunicación I2C (por ejemplo, 1 ESP8266 y 1 Arduino UNO).
* Sensor de temperatura LM35 o TMP36.
* Un LED como actuador.
* Cuenta en ThingSpeak para visualización de datos.
* Conexión a Internet para el envío de información.
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

2. **Nodo Maestro (ESP8266 o Arduino con WiFi):**
   Recibe la temperatura del esclavo, la muestra en el monitor serial y la envía a ThingSpeak para su almacenamiento y visualización.

3. **Plataforma ThingSpeak:**
   Muestra en tiempo real el valor actual de la temperatura, genera una gráfica histórica y permite analizar tendencias.

---

## Código Fuente

A continuación se incluyen los fragmentos de código utilizados para la implementación.
(Deja este espacio para insertar tus archivos `.ino` de Arduino o ESP8266).

### Código del Nodo Esclavo (Sensor)

```cpp
// Código pendiente de inserción
```

### Código del Nodo Maestro (WiFi y Envío a ThingSpeak)

```cpp
// Código pendiente de inserción
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

*(Inserta aquí las imágenes del dashboard y las gráficas generadas)*



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

## Referencias

* ThingSpeak IoT Analytics Platform. [https://thingspeak.com](https://thingspeak.com)
* Arduino Documentation. [https://www.arduino.cc/reference](https://www.arduino.cc/reference)
* Espressif ESP8266 AT Commands Set.
* Sensor LM35 Datasheet.
