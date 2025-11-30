#include <WiFi.h>
#include <vector>
#include "connector.h"
#include <Arduino.h>

// Static IP configuration
IPAddress local_ip(192, 168, 1, 1); // Set your desired IP address
IPAddress gateway(192, 168, 1, 1);  // Gateway (usually the same as the local IP for AP mode)
IPAddress subnet(255, 255, 255, 0); // Subnet mask

bool configureHotspot()
{
    if (WiFi.softAPConfig(local_ip, gateway, subnet))
        return true;

    Serial.println("Failed to configure static IP!");
    return false;
}

bool createHotspot(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_AP_STA);
    // Start the Access Point
    Serial.print("Creating WiFi hotspot: "); Serial.println(ssid); 
    Serial.print("Password: "); Serial.println(password);

    bool ap = WiFi.softAP(ssid, password, 6);
    delay(500);

    if (ap) {
        Serial.println("AP created: " + WiFi.softAPIP().toString());
        return true;
    };

    return false;
}

void displayHotspotInfo()
{
    Serial.println("Wi-Fi Hotspot Created! Access Point IP: " +
                   WiFi.softAPIP().toString());
}

// std::vector<String> getAvailableNetworks()
// {
//     std::vector<String> networks;
//
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect();
//
//     delay(500); // Give time for WiFi to initialize
//
//     Serial.println("Scanning WiFi networks...");
//     int n = WiFi.scanNetworks();
//
//     if (n == 0)
//         return {};
//
//     for (int i = 0; i < n; i++)
//         networks.push_back(String(i) + ": " + WiFi.SSID(i));
//
//     // Clear the results from memory
//     WiFi.scanDelete();
//
//     return networks;
// }

bool connectToNetwork(const char *ssid, const char *password)
{
    int maxTries = 3;

    // Start Wi-fi in STA (Station) mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Disconnect from any previously connected network
    delay(100);

    // Connect to the network
    WiFi.begin(ssid, password);

    Serial.println("Connecting to Wi-Fi...");
    int retries = 0;

    while (WiFi.status() != WL_CONNECTED && retries < maxTries)
    {
        delay(1000);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Failed to connect to the network!");
        return false;
    }

    Serial.println(WiFi.localIP());
    return true;
}

bool isConnected()
{
    if (WiFi.status() == WL_CONNECTED)
        return true;
    return false;
}