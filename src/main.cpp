#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>
#include <string.h>

void PreHTTPSRedirect();
void HTTPSRedirectConnect();
void wifiConnect();
void heapAndStack();
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

//Test String
boolean toggle = false;  // Control HTTPSRedirect
boolean toggle1 = false; // Control wifi connect
char test_GET[] = "English for Communication,\tFri Jun 14 2019 16:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tMon Jun 17 2019 12:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tTue Jun 18 2019 12:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tWed Jun 19 2019 12:00:00 GMT+0700 (ICT)\nEnglish for Communication ,\tThu Jun 20 2019 16:00:00 GMT+0700 (ICT)\n";
int i = 0;
char split_char[20][1000];
char title[20][1000];
char day[20][2];
char month[20][2];
char year[20][4];
char endTime[20][8];
char *pch;
unsigned int x_index = 0;
unsigned int y_index = 0;
void setup()
{
   // put your setup code here, to run once:
   Serial.begin(115200);
   Serial.flush();

   heapAndStack();

   if (toggle1)
   {
      wifiConnect();
   }

   if (toggle)
   {
      PreHTTPSRedirect();
   }
   Serial.println(test_GET);
   pch = strtok(test_GET, "\t\n");
   Serial.print("\nWhile spilt responbody : [");
   while (pch != NULL)
   {
      //Serial.println(pch);
      for (x_index = 0; x_index < (unsigned)strlen(pch); x_index++)
      {

         split_char[y_index][x_index] = pch[x_index];
      }
      Serial.print("=");
      pch = strtok(NULL, "\t\n");
      y_index++;
   }
   Serial.print("]  100% ");
   Serial.println();
   Serial.println(">>>>> split string <<<<<");
   Serial.println("=========================");

   for (y_index = 0; y_index < 20; y_index++)
   {
      if (split_char[y_index][0] != '\0')
      {
         Serial.printf("[ %u ] ", y_index);
         for (x_index = 0; x_index < 100; x_index++)
         {
            if (split_char[y_index][x_index] != '\0')
            {

               Serial.print(split_char[y_index][x_index]);
            }
            else
            {
               break;
            }
         }
         Serial.println();
      }
   }
   Serial.println("============");

   Serial.println();
   Serial.println("Title list ");
   for (y_index = 0; y_index < 20; y_index++)
   {
      if (y_index % 2 == 0 && split_char[y_index][0] != '\0')
      {
         Serial.printf("[ %u ] ", y_index);
         for (x_index = 0; x_index < 100; x_index++)
         {
            if (split_char[y_index][x_index] != '\0')
            {

               Serial.print(split_char[y_index][x_index]);
            }
            else
            {
               break;
            }
         }
         Serial.println();
      }
   }
}

void loop()
{
   if (toggle)
   {
      HTTPSRedirectConnect();
   }
   delay(1000);
}

void heapAndStack()
{
   free_heap_before = ESP.getFreeHeap();
   free_stack_before = ESP.getFreeContStack();

   Serial.printf("\nFree heap: %u\n", free_heap_before);
   Serial.printf("Free stack: %u\n", free_stack_before);
}
void wifiConnect()
{
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
}
void PreHTTPSRedirect()
{
   //USE HTTPSRedirect to create a new TLS connection
   client = new HTTPSRedirect(httpsPort);
   client->setInsecure();
   client->setPrintResponseBody(false);
   client->setContentTypeHeader("application/json");

   Serial.print("Connecting to : ");
   Serial.println(host);
   bool flag = false;
   for (int i = 0; i < 5; i++)
   {
      int retval = client->connect(host, httpsPort);
      if (retval == 1)
      {
         flag = true;
         Serial.println("Connection success ...");
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

   Serial.print("\nConnecting to google calendar ...");
   Serial.println("\n=============================");
   client->GET(url, host);
   delete client;
   client = nullptr;
}

void HTTPSRedirectConnect()
{
   static int error_count = 0;
   static int connect_count = 0;
   const int MAX_CONNECT = 20;
   static bool flag = false;
   String get = "";

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
         client->GET(url, host);
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
      get += client->getResponseBody();
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

   Serial.println();
   Serial.println("GET Respon body : ");
   Serial.println("===================");
   Serial.println(get);
   Serial.println("===================");

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
}