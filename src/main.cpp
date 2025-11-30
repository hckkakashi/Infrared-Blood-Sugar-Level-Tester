#include <Arduino.h>
#include "connector.h"
#include "ConfigServer.h"

int irSensorPin = 34;    // IR Receiver analog output
int irEmitterPin = 13;   // IR LED pin

// PWM parameters for IR LED
const int pwmChannel = 0;
const int pwmFreq = 38000; // 38 kHz
const int pwmResolution = 8; // 8-bit duty cycle

// PLSR parameters from trained model
const float W = 0.04217854635964616;
const float b = 165.59544224394025;
const float mean_ADC = 2057.7625;
const float std_ADC = 1183.1341317001002;

// Store last and history of glucose readings
float lastGlucose = 0.0;
std::vector<float> glucoseHistory;

// Flag to trigger single measurement
bool measureNow = false;

ConfigServer* server = nullptr;

// --- IR LED control ---
void setupIR()
{
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(irEmitterPin, pwmChannel);
}

void turnIREmitterOn(uint8_t duty)
{
    ledcWrite(pwmChannel, duty);
}

void turnIREmitterOff()
{
    ledcWrite(pwmChannel, 0);
}

// --- Take multiple ADC samples and return median ---
int readIRMedian(int numSamples = 30)
{
    int samples[numSamples];

    for (int i = 0; i < numSamples; i++) {
        samples[i] = analogRead(irSensorPin);
        delay(5);
    }

    // Sort samples to get median
    for (int i = 0; i < numSamples - 1; i++) {
        for (int j = i + 1; j < numSamples; j++) {
            if (samples[j] < samples[i]) {
                int temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }

    return samples[numSamples / 2]; // median value
}

// --- Glucose calculation ---
float calculateGlucose(int rawADC)
{
    float normalized = (rawADC - mean_ADC) / std_ADC;
    return W * normalized + b;
}

// --- Get stable reading with baseline subtraction ---
int getStableReading()
{
    const uint8_t intensities[] = {50, 80, 120, 160}; // LED intensities
    const int attempts = sizeof(intensities)/sizeof(intensities[0]);
    const int threshold = 50; // minimum ADC difference to consider valid

    // Measure baseline (LED off)
    int adcBaseline = readIRMedian();

    for (int i = 0; i < attempts; i++) {
        turnIREmitterOn(intensities[i]);
        delay(400); // allow sensor to stabilize
        int adcLED = readIRMedian();
        turnIREmitterOff();

        int adcCorrected = adcLED - adcBaseline;

        if (adcCorrected > threshold && adcCorrected < 4095) {
            return adcCorrected; // valid reading
        }
    }

    return -1; // failed to get valid reading
}

// --- Setup configuration server and hotspot ---
void runConfiguration()
{
    server = new ConfigServer();
    configureHotspot();
    createHotspot("GlucoseTester", "12345678");

    server->setOnRestart([](){
        glucoseHistory.clear();
        lastGlucose = 0;
        ESP.restart();
    });

    Serial.println("Server running on: http://192.168.1.1");
    server->start();
}

void setup()
{
    Serial.begin(9600);

    setupIR();
    turnIREmitterOff(); // ensure LED off initially

    runConfiguration();
    delay(500);
}

void loop()
{
    if (measureNow)
    {
        int adcValue = getStableReading();
        if (adcValue != -1) {
            lastGlucose = calculateGlucose(adcValue);
            glucoseHistory.push_back(lastGlucose);

            Serial.print("ADC (corrected): "); Serial.print(adcValue);
            Serial.print(" => Glucose: "); Serial.println(lastGlucose);
        } else {
            Serial.println("No valid measurement detected. Please place sample correctly.");
        }

        measureNow = false;
    }

    delay(100);
}
