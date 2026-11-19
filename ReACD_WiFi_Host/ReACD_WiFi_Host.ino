
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ap_ssid = "SchulungsnetzTHX1138";
const char* ap_password = "andromeda";
IPAddress ip = IPAddress(192,168,178,1);
IPAddress nmask = IPAddress(255,255,255,0);
const byte DNS_PORT = 53;

WebServer server(80);

DNSServer dnsServer;



void handleConnectTest() {
  // Windows Connect Test Endpoint
  server.send(200, "text/plain", "Microsoft Connect Test");
}

void handleSecret()
 {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Illuminati</title>
  </head>
  <body style="font-family:Arial;text-align:center;padding:30px">
    <h1>Willkommen auf der Geheimseite der ILLUMINATI</h1>
    <p>Der Hotspot ist...</p>
    <p>100%ig Illuminati confirmed.</p>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Willkommen</title>
  </head>
  <body style="font-family:Arial;text-align:center;padding:30px">
    <h1>Willkommen auf der Startseite des Schulungsnetzes!</h1>
    <p>Der Hotspot ist...</p>
    <p>100%ig einsatzbereit.</p>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.sendHeader("Location", "http://192.168.178.1/", true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starting ESP32 access point...");

  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  // Configure access point
  if (!WiFi.softAP(ap_ssid, ap_password)) {
    Serial.println("ERROR: Failed to start hotspot!");
    while (true) {
      delay(1000);
    }
  }

  //IPAddress ip = WiFi.softAPIP();
  
  WiFi.softAPConfig(ip,ip,nmask);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  Serial.println("Hotspot started!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);

  Serial.print("Password: ");
  Serial.println(ap_password);

  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("MAC address: ");
  Serial.println(WiFi.softAPmacAddress());


  server.on("/connecttest.txt", handleConnectTest);
  server.on("/ncsi.txt", handleConnectTest);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/secret.html", HTTP_GET, handleSecret);
  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started!");

}

void loop() {
  
  dnsServer.processNextRequest();
  server.handleClient();
}
