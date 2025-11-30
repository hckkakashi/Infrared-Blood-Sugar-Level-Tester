#include "ConfigServer.h"
#include "connector.h"
#include <vector>

extern float lastGlucose;
extern std::vector<float> glucoseHistory;
extern bool measureNow;

ConfigServer::ConfigServer() : server(80) { }

void ConfigServer::start()
{
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request){ this->handleRoot(request); });
    server.on("/restart", HTTP_GET, [this](AsyncWebServerRequest* request){ this->handleRestart(request); });
    server.on("/startTest", HTTP_GET, [this](AsyncWebServerRequest* request){ this->handleStartTest(request); });

    server.begin();
}

void ConfigServer::setOnRestart(const std::function<void()>& callback)
{
    callbackFunction = callback;
}

void ConfigServer::handleRoot(AsyncWebServerRequest* request)
{
    String html = "<!DOCTYPE html><html><head><title>Glucose Tester</title>";
    html += "<meta http-equiv='refresh' content='5'>";
    html += "<style>body{font-family:Arial;text-align:center;} table{margin:auto;border-collapse:collapse;} td,th{padding:5px;border:1px solid #000;}</style>";
    html += "</head><body>";
    html += "<h1>Infrared Glucose Tester</h1>";

    html += "<h2>Last Glucose: " + String(lastGlucose) + " mg/dL</h2>";

    html += "<h2>History</h2>";
    html += "<table><tr><th>#</th><th>Value (mg/dL)</th></tr>";
    for (size_t i = 0; i < glucoseHistory.size(); i++) {
        html += "<tr><td>" + String(i+1) + "</td><td>" + String(glucoseHistory[i]) + "</td></tr>";
    }
    html += "</table>";

    html += "<br><button onclick=\"location.href='/startTest'\">Take Measurement</button>";
    html += "<br><br><button onclick=\"location.href='/restart'\">Restart Device</button>";

    html += "</body></html>";

    request->send(200, "text/html", html);
}

void ConfigServer::handleStartTest(AsyncWebServerRequest* request)
{
    measureNow = true; // trigger main loop to take one measurement
    String html = "<!DOCTYPE html><html><head><title>Measurement Started</title>";
    html += "<meta http-equiv='refresh' content='1;url=/'></head><body>";
    html += "<h1>Measurement Taken! See the result on main page.</h1>";
    html += "</body></html>";
    request->send(200, "text/html", html);
}

void ConfigServer::handleRestart(AsyncWebServerRequest* request)
{
    String html = "<!DOCTYPE html><html><head><title>Restarting...</title></head><body>";
    html += "<h1>Restarting device...</h1></body></html>";
    request->send(200, "text/html", html);

    if (callbackFunction) callbackFunction();
}
