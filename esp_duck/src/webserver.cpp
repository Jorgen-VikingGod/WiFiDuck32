/*
   This software is licensed under the MIT License. See the license file for details.
   Source: https://github.com/spacehuhntech/WiFiDuck
 */

#include "webserver.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPIFFSEditor.h>

#include "config.h"
#include "debug.h"
#include "cli.h"
#include "spiffs.h"
#include "settings.h"

#include "webfiles.h"

void reply(AsyncWebServerRequest* request, int code, const char* type, const uint8_t* data, size_t len) {
  AsyncWebServerResponse* response = request->beginResponse_P(code, type, data, len);

  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

namespace webserver {
// ===== PRIVATE ===== //
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

AsyncWebSocketClient* currentClient{nullptr};

DNSServer dnsServer;

bool reboot = false;

void wsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data,
             size_t len) {
  if (type == WS_EVT_CONNECT) {
    debugf("WS Client connected %u\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    debugf("WS Client disconnected %u\n", client->id());
  } else if (type == WS_EVT_ERROR) {
    debugf("WS Client %u error(%u): %s\n", client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_PONG) {
    debugf("PONG %u\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->opcode == WS_TEXT) {
      char* msg = (char*)data;
      msg[len] = 0;
      debugf("Message from %u [%llu byte]=%s", client->id(), info->len, msg);
      currentClient = client;
      cli::parse(
          msg,
          [](const char* str) {
            webserver::send(str);
            debugf("%s\n", str);
          },
          false);
      currentClient = nullptr;
    }
  }
}

void saveConfigToJSON() {
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove("/config.js");
  // Open file for writing
  File file = SPIFFS.open("/config.js", FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  file.print("var ip = '" + WiFi.localIP().toString() + "';");
  debugln("var ip = '" + WiFi.localIP().toString() + "';");
  // Close the file
  file.close();
}

// ===== PUBLIC ===== //
void begin() {
  // Access Point
  WiFi.setHostname(HOSTNAME);

  WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  WiFi.begin(STA_SSID, STA_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(STA_SSID, STA_PASSWORD);
  }
  saveConfigToJSON();

  MDNS.addService("http", "tcp", 80);
  // SPIFFS editor on /edit
  server.addHandler(new SPIFFSEditor(SPIFFS, EDITOR_USER, EDITOR_PASSWORD));
  // Websocket
  ws.onEvent(wsEvent);
  server.addHandler(&ws);

  // Webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { request->redirect("/index.html"); });
  server.serveStatic("/config.js", SPIFFS, "/config.js");
  server.onNotFound([](AsyncWebServerRequest* request) { request->redirect("/error404.html"); });
  server.on("/run", [](AsyncWebServerRequest* request) {
    String message;
    if (request->hasParam("cmd")) {
      message = request->getParam("cmd")->value();
    }
    request->send(200, "text/plain", "Run: " + message);
    cli::parse(
        message.c_str(), [](const char* str) { debugf("%s\n", str); }, false);
  });
  WEBSERVER_CALLBACK;
  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest* request) {
        reboot = !Update.hasError();
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", reboot ? "OK" : "FAIL");
        response->addHeader("Connection", "close");
        request->send(response);
      },
      [](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
        if (!index) {
          debugf("Update Start: %s\n", filename.c_str());
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
          }
        }
        if (!Update.hasError()) {
          if (Update.write(data, len) != len) {
            Update.printError(Serial);
          }
        }
        if (final) {
          if (Update.end(true)) {
            debugf("Update Success: %uB\n", index + len);
          } else {
            Update.printError(Serial);
          }
        }
      });
  // Start Server
  server.begin();
  debugln("Started Webserver");
}

void update() {
  ws.cleanupClients();
  if (reboot) ESP.restart();
}

void send(const char* str) {
  if (currentClient) currentClient->text(str);
}
}  // namespace webserver