#include <DHT11.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include <SPI.h>
#include <SD.h>

DHT11 dht11(3);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

const int chipSelect = 10;

// 10 mins = 600000 ms
const unsigned long LOG_INTERVAL = 600000;
unsigned long lastLogTime = 0;

void setup() {
  Serial.begin(9600);

  if (!tsl.begin()) {
    Serial.println("No TSL2591 found");
    while (1);
  }

  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  tsl.setGain(TSL2591_GAIN_MED);

  if (!SD.begin(chipSelect)) {
    Serial.println("SD init failed!");
    while (1);
  }

  Serial.println("SD card initialized.");

  // Write header 
  File dataFile = SD.open("env_log.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time(ms),Temp(C),Humidity(%),IR,Visible,Lux");
    dataFile.close();
  }
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastLogTime >= LOG_INTERVAL) {
    lastLogTime = currentTime;

    // Read DHT11 
    int temperature = 0;
    int humidity = 0;
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Read Light Sensor 
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir = lum >> 16;
    uint16_t full = lum & 0xFFFF;
    uint16_t visible = full - ir;
    float lux = tsl.calculateLux(full, ir);

    /* Build data string */

    String dataString = "";
    dataString += "Time(ms): " + String(currentTime);
    dataString += " | Temp(C): " + String(temperature);
    dataString += " | Humidity(%): " + String(humidity);
    dataString += " | IR: " + String(ir);
    dataString += " | Visible: " + String(visible);
    dataString += " | Lux: " + String(lux);

    /* Write to SD card */
    File dataFile = SD.open("env_log.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println("Logged: " + dataString);
    } else {
      Serial.println("Error opening file");
    }
  }
}