#pragma once
#include <ESPAsyncWebServer.h>
#include <functional>
#include <vector>
#include <Arduino.h>

class ConfigServer
{
public:
    ConfigServer();
    void start();
    void setOnRestart(const std::function<void()>& callback);

private:
    AsyncWebServer server;
    std::function<void()> callbackFunction;

    void handleRoot(AsyncWebServerRequest* request);
    void handleRestart(AsyncWebServerRequest* request);
    void handleStartTest(AsyncWebServerRequest* request);
};
