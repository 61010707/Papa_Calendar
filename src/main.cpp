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
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  Serial.print("COnnecting to : ");
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++)
  {
    int retval = client->connect(host, httpsPort);
    if (retval == 1)
    {
      flag = true;
      break;
    }
    else
    {
      Serial.println("Connection failed. Retrying ... ");
    }
  }

  if (!flag)
  {
    Serial.print("Could not connect to server");
    Serial.println(host);
    Serial.println("Exiting...");
  }

  if (client->setFingerprint(fingerPrint))
  {
    Serial.println("Certificate match.");
  }
  else
  {
    Serial.println("Certificate mis-match");
  }

  client->GET(url, host);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
  delete client;
  client = nullptr;
}

void loop()
{
  static int error_count = 0;
  static int connect_count = 0;
  const int MAX_CONNECT = 20;
  static bool flag = false;

  if (!flag)
  {
    free_heap_before = ESP.getFreeHeap();
    free_stack_before = ESP.getFreeContStack();
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
    flag = true;
  }

  if (client != nullptr)
  {
    if (!client->connected())
    {
      client->connect(host, httpsPort);
      //client->GET(url, host);
    }
  }
  else
  {
    Serial.println("Error creating client object !");
    error_count = 5;
  }

  if (connect_count > MAX_CONNECT)
  {
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }

  if (client->GET(url, host))
  {
    
    ++connect_count;
    Serial.print("Connect count while connecting :");
    Serial.println(connect_count);
  }
  else
  {
    ++error_count;
    Serial.print("Error-count while connecting :");
    Serial.println(error_count);
  }

  if (error_count > 3)
  {
    Serial.println("Halting processor...");
    delete client;
    Serial.printf("Final free heap: %u\n", ESP.getFreeHeap());
    Serial.printf("Final stack: %u\n", ESP.getFreeContStack());
    Serial.flush();
    ESP.deepSleep(0);
  }

  delay(4000);
  delay(50);
}