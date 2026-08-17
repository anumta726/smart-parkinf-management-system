#include <WiFi.h>
#include <WebServer.h>

#include "index.h"
#include "style.h"
#include "script.h"

WebServer server(80);

// ---------- WiFi AP ----------

const char* ssid = "Smart Parking";
const char* password = "12345678";

// ---------- Parking Data ----------

String slot1 = "Empty";
String slot2 = "Empty";
String slot3 = "Empty";
String slot4 = "Empty";

int available = 4;

String recommend = "S1";

String packet = "";

// ---------- Read Arduino ----------

void readArduinoData()
{
  while (Serial2.available())
  {
    char c = Serial2.read();

    if (c == '\n')
    {
      packet.trim();

      int p1 = packet.indexOf(',');
      int p2 = packet.indexOf(',', p1 + 1);
      int p3 = packet.indexOf(',', p2 + 1);
      int p4 = packet.indexOf(',', p3 + 1);
      int p5 = packet.indexOf(',', p4 + 1);

      if (p1 > 0 && p2 > 0 && p3 > 0 && p4 > 0 && p5 > 0)
      {
        slot1 = packet.substring(0, p1) == "1" ? "Empty" : "Occupied";

        slot2 = packet.substring(p1 + 1, p2) == "1" ? "Empty" : "Occupied";

        slot3 = packet.substring(p2 + 1, p3) == "1" ? "Empty" : "Occupied";

        slot4 = packet.substring(p3 + 1, p4) == "1" ? "Empty" : "Occupied";

        available = packet.substring(p4 + 1, p5).toInt();

        recommend = packet.substring(p5 + 1);
      }

      packet = "";
    }
    else
    {
      packet += c;
    }
  }
}

// ---------- JSON Data ----------

void handleData()
{
  String json = "{";

  json += "\"slot1\":\"" + slot1 + "\",";
  json += "\"slot2\":\"" + slot2 + "\",";
  json += "\"slot3\":\"" + slot3 + "\",";
  json += "\"slot4\":\"" + slot4 + "\",";
  json += "\"available\":" + String(available) + ",";
  json += "\"recommend\":\"" + recommend + "\"";

  json += "}";

  server.send(200, "application/json", json);
}

// ---------- Web Pages ----------

void handleRoot()
{
  server.send_P(200, "text/html", index_html);
}

void handleCSS()
{
  server.send_P(200, "text/css", style_css);
}

void handleJS()
{
  server.send_P(200, "application/javascript", script_js);
}

// ---------- SETUP ----------

void setup()
{
  Serial.begin(115200);

  // RX2 = GPIO16
  // TX2 = GPIO17
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("SMART PARKING SYSTEM");
  Serial.println("-----------------------");
  Serial.print("WiFi : ");
  Serial.println(ssid);

  Serial.print("IP : ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/style.css", handleCSS);
  server.on("/script.js", handleJS);
  server.on("/data", handleData);

  server.begin();

  Serial.println("Web Server Started");
}

// ---------- LOOP ----------

void loop()
{
  readArduinoData();

  server.handleClient();
}
