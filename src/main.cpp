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
char month[20][10];
char year[20][10];
char endTime[20][10];
int x_size = sizeof(split_char[0]);
int y_size = sizeof(split_char) / x_size;
// design from long code to funtion
// have variable
//    split_char = for save ssplited char from responbody
//    title
//    day = day in text
//    date = day in number
//    month
//    year
//    endTime
// design function
//    1 split char funtion recieve : base char , split_char  , delimeter
//    2 split char to each char array : base char ,   each cahr
// ref to issue 18
void split_str(char *in_str, char *out_str, int x_size, int y_size, char delimiter[])
{
   char *pch = strtok(in_str, delimiter);
   int y_index = 0;
   while (pch != NULL)
   {
      Serial.println(pch);
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
         y_title = i == 0 ? y_title : ++y_title;
         strcpy(title[y_title], split_char[i]);
      }
      else
      {
         y_date = i == 1 ? y_date : ++y_date;
         strcpy(split_Date[y_date], split_char[i]);
      }
   }
}
void setup()
{
   // put your setup code here, to run once:
   Serial.begin(115200);
   Serial.flush();

   Serial.printf("x_index : %d , y_index : %d\n", x_size, y_size);
   split_str(test_GET, (char *)split_char, x_size, y_size, "\t\n");
   Serial.println();
   filter_even_odd();
   for (int i = 0; i < 20; i++)
   {
      if (title[i][0] != '\0' || split_Date[i][0] != '\0')
      {
         Serial.printf("[ %d ] %s\n", i, title[i]);
         Serial.printf("[ %d ] %s\n", i, split_Date[i]);
      }
   }

   for (int y_index = 0, number = 0; y_index < sizeof(split_Date) / sizeof(split_Date[0]); y_index++)
   {
      if (split_Date[y_index][0] != '\0')
      {
         Serial.printf("[ %d ] ", y_index);
         char *str;
         str = strtok(split_Date[y_index], " ");
         while (str != NULL)
         {
            Serial.printf("\t%d\t%s\n", number++, str);
            str = strtok(NULL, " ");
         }
         number = 0;
      }
   }
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

/*void refStringFunc()
{
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
   Serial.println("=======================================");
   Serial.println();
   Serial.println("Title list from split string ");
   int y_title = 0;
   for (y_index = 0; y_index < 20; y_index++)
   {
      if (y_index % 2 == 0 && split_char[y_index][0] != '\0')
      {
         Serial.printf("[ %u ] ", y_title);
         for (x_index = 0; x_index < 100; x_index++)
         {
            if (split_char[y_index][x_index] != '\0')
            {
               title[y_title][x_index] = split_char[y_index][x_index];
               Serial.print(title[y_title][x_index]);
            }
            else
            {
               break;
            }
         }
         Serial.println();
         y_title++;
      }
   }

   Serial.println("=======================================");
   Serial.println();
   Serial.println("End Date list from split string");

   int y_endDate = 0;
   for (y_index = 0; y_index < 20; y_index++)
   {
      if (y_index % 2 != 0 && split_char[y_index][0] != '\0')
      {
         Serial.printf("[ %u ] ", y_endDate);
         for (x_index = 0; x_index < 100; x_index++)
         {
            if (split_char[y_index][x_index] != '\0')
            {
               endDate[y_endDate][x_index] = split_char[y_index][x_index];
               Serial.print(endDate[y_endDate][x_index]);
            }
            else
            {
               break;
            }
         }
         Serial.println();
         y_endDate++;
      }
   }

   Serial.println();
   Serial.println("while split date...");
   for (int y_splitDate = 0; y_splitDate < 20; y_splitDate++)
   {

      if (endDate[y_splitDate][0] != '\0')
      {
         int dateIndex = 0;
         Serial.printf("[ date %d ]\n", y_splitDate);
         pch = strtok(endDate[y_splitDate], " ");
         while (pch != NULL)
         {
            Serial.printf("index %d :", dateIndex);
            Serial.println(pch);
            switch (dateIndex)
            {
            case 0:
               Serial.print("\tin day :");
               for (unsigned int x_day = 0; x_day < strlen(pch); x_day++)
               {
                  day[y_splitDate][x_day] = pch[x_day];
                  Serial.print(day[y_splitDate][x_day]);
               }
               Serial.println();
               break;
            case 1:
               Serial.print("\tin month :");
               for (unsigned int x_month = 0; x_month < strlen(pch); x_month++)
               {
                  month[y_splitDate][x_month] = pch[x_month];
                  Serial.print(month[y_splitDate][x_month]);
               }
               Serial.println();
               break;
            case 2:
               Serial.print("\tin date :");
               for (unsigned int x_in_loop = 0; x_in_loop < strlen(pch); x_in_loop++)
               {
                  date[y_splitDate][x_in_loop] = pch[x_in_loop];
                  Serial.print(date[y_splitDate][x_in_loop]);
               }
               Serial.println();
               break;
            case 3:
               Serial.print("\tin year :");
               for (unsigned int x_in_loop = 0; x_in_loop < strlen(pch); x_in_loop++)
               {
                  year[y_splitDate][x_in_loop] = pch[x_in_loop];
                  Serial.print(year[y_splitDate][x_in_loop]);
               }
               Serial.println();
               break;
            case 4:
               Serial.print("\tin endTime :");
               for (unsigned int x_in_loop = 0; x_in_loop < strlen(pch); x_in_loop++)
               {
                  endTime[y_splitDate][x_in_loop] = pch[x_in_loop];
                  Serial.print(endTime[y_splitDate][x_in_loop]);
               }
               Serial.println();
               break;
            default:
               break;
            }

            pch = strtok(NULL, " ");
            dateIndex++;
         }
         Serial.println("=============================");
      }
   }
}
 */
