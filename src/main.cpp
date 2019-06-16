#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>
#include <string.h>
// add some comment
void PreHTTPSRedirect();
void HTTPSRedirectConnect();
void wifiConnect();
void heapAndStack();
void refStringFunc();
void split_str(char *in_str, char *out_str, int x_size, int y_size, char delimiter[]);
void filter_even_odd();
void date_info();
void print_info();
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
char test_GET[1000] = "English for Communication,\tFri Jun 14 2019 16:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tMon Jun 17 2019 12:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tTue Jun 18 2019 12:00:00 GMT+0700 (ICT)\nThai Society and Culture ,\tWed Jun 19 2019 12:00:00 GMT+0700 (ICT)\nEnglish for Communication ,\tThu Jun 20 2019 16:00:00 GMT+0700 (ICT)\n";
char split_char[20][1000];
char split_Date[20][100];
char title[20][1000];
char day[20][10];
char date[20][10];
char month[20][10];
char year[20][10];
char endTime[20][10];
int date_int[20];
int x_size = sizeof(split_char[0]);
int y_size = sizeof(split_char) / x_size;

void str_analysis()
{
   split_str(test_GET, (char *)split_char, x_size, y_size, "\t\n");
   filter_even_odd();
   date_info();
}
void setup()
{
   // put your setup code here, to run once:
   Serial.begin(115200);
   Serial.flush();
str_analysis();
   print_info();
}

void loop()
{
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

void split_str(char *in_str, char *out_str, int x_size, int y_size, char delimiter[])
{
   char *pch = strtok(in_str, delimiter);
   int y_index = 0;
   while (pch != NULL)
   {
      strcpy((out_str + (y_index * x_size)), pch);
      y_index++;
      pch = strtok(NULL, delimiter);
   }
}
void filter_even_odd()
{
   for (int i = 0, y_title = 0, y_date = 0; i < y_size; i++)
   {
      if (i % 2 == 0)
      {
         y_title = (i == 0 ? y_title : ++y_title);
         strcpy(title[y_title], split_char[i]);
      }
      else
      {
         y_date = i == 1 ? y_date : ++y_date;
         strcpy(split_Date[y_date], split_char[i]);
      }
   }
}

void date_info()
{
   String int_str;
   for (unsigned int y_index = 0, number = 0; y_index < sizeof(split_Date) / sizeof(split_Date[0]); y_index++)
   {
      if (split_Date[y_index][0] != '\0')
      {
         Serial.printf("[ %d ] ", y_index);
         char *str;
         str = strtok(split_Date[y_index], " ");
         while (str != NULL)
         {
            number++;
            switch (number)
            {
            case 1:
               strcpy(day[y_index], str);
               break;
            case 2:
               strcpy(month[y_index], str);
               break;
            case 3:
               strcpy(date[y_index], str);
               int_str = str;
               date_int[y_index] = int_str.toInt();
               break;
            case 4:
               strcpy(year[y_index], str);

               break;
            case 5:
               strcpy(endTime[y_index], str);
               break;
            default:
               break;
            }
            str = strtok(NULL, " ");
         }
         number = 0;
      }
   }
}

void print_info()
{
   for (int i = 0; i < 20; i++)
   {
      if (title[i][0] != '\0' || split_Date[i][0] != '\0')
      {
         Serial.printf("[ %d ] Titile\t%s\n", i, title[i]);
         Serial.printf("[ %d ] split_date\t%s\n", i, split_Date[i]);
         Serial.printf("[ %d ] day\t%s\n", i, day[i]);
         Serial.printf("[ %d ] month\t%s\n", i, month[i]);
         Serial.printf("[ %d ] date\t%s\n", i, date[i]);
         Serial.printf("[ %d ] year\t%s\n", i, year[i]);
         Serial.printf("[ %d ] endTime\t%s\n", i, endTime[i]);
         Serial.printf("[ %d ] date_int\t%d\n", i, date_int[i]);
      }
   }
}