#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

int irSensorPin = 34;    // IR Receiver pin
int irEmitterPin = 13;   // IR LED pin

// PLSR parameters from trained model
const float W = 0.04217854635964616;
const float b = 165.59544224394025;
const float mean_ADC = 2057.7625;
const float std_ADC = 1183.1341317001002;

// WiFi credentials
const char* ssid = "Ujamaa WIFI Front 5G";
const char* password = "Ujamaa@1234";

// Web server on port 80
WebServer server(80);

// Function to measure glucose
float measureGlucose() {
    digitalWrite(irEmitterPin, HIGH);
    delay(50);  // let LED stabilize

    int adcRaw = analogRead(irSensorPin);

    digitalWrite(irEmitterPin, LOW);

    // Apply PLSR formula
    float glucose = W * ((float)adcRaw - mean_ADC) / std_ADC + b;

    return glucose;
}

// Web endpoint
void handleGlucose() {
    float glucose = measureGlucose();
    String json = "{ \"glucose\": " + String(glucose, 2) + " }";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(9600);

    pinMode(irEmitterPin, OUTPUT);
    digitalWrite(irEmitterPin, LOW);
    pinMode(irSensorPin, INPUT);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    // Setup API endpoint
    server.on("/getGlucose", handleGlucose);
    server.begin();
}

void loop() {
    server.handleClient();
}
