#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>

// Network setup
const char *ssid = "DrZin";
const char *pass = "0985626152a";

// HTTPS setup
const char *host = "script.google.com";
const int httpsPort = 443;
HTTPSRedirect* client = nullptr;
const char *fingerPrint = "B1:E0:38:3F:63:8B:82:45:E0:90:72:01:80:B8:8D:04:58:34:08:F8";

// Google script setup
const char *GScriptId = "AKfycbwfRU5WMwEGKGKftI3Gwj4nCSVigayzfQwInbnjTaHeVpj1-rJy";
String url = String("/macros/s/") + GScriptId + "/exec";

// common variables

void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}