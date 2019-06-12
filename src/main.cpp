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
HTTPSRedirect *client = nullptr;
const char *fingerPrint = "B1:E0:38:3F:63:8B:82:45:E0:90:72:01:80:B8:8D:04:58:34:08:F8";

// Google script setup
const char *GScriptId = "AKfycbwfRU5WMwEGKGKftI3Gwj4nCSVigayzfQwInbnjTaHeVpj1-rJy";
String url = String("/macros/s/") + GScriptId + "/exec";

// common variables

unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.flush();

  free_heap_before = ESP.getFreeHeap();
  free_stack_before = ESP.getFreeContStack();

  Serial.printf("Free heap: %u\n", free_heap_before);
  Serial.printf("Free stack: %u\n", free_stack_before);

  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    /* code */
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //USE HTTPSRedirect to create a new TLS connection
  
}

void loop()
{
  // put your main code here, to run repeatedly:
}