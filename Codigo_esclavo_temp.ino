#include <Wire.h>

#define SLAVE_ADDR 8     // Dirección I2C del esclavo
#define SENSOR_PIN A0    // Pin donde está el LM35

float temperature = 0;

void setup() {
  Wire.begin(SLAVE_ADDR);   // Iniciar como esclavo I2C
  Wire.onRequest(sendD6ata); // Función que se llama cuando el maestro pide datos
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
