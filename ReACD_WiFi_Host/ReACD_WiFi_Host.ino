
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char* ap_ssid = "ReACD-Schulungsnetz";
const char* ap_password = "andromeda";

IPAddress ip(192, 168, 178, 1);
IPAddress nmask(255, 255, 255, 0);

const byte DNS_PORT = 53;

ESP8266WebServer server(80);
DNSServer dnsServer;

const int readyIndicatorPin = 5; //GPIO Pin 5(labeled as D1)

void handleConnectTest()
{
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
        <p>Das Netzwerk ist...</p>
        <p>100%ig Illuminati confirmed.</p>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", html);
}

void handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Willkommen</title>
    </head>
    <body style="font-family:Arial;text-align:center;padding:30px">
        <h1>Willkommen auf der Startseite des ReACD-Schulungsnetzes!</h1>
        <p>Das Netzwerk ist...</p>
        <p>100%ig einsatzbereit.</p>
    </body>
    </html>
    )rawliteral";

    server.send(200, "text/html", html);
}

void handleNotFound()
{
    server.sendHeader("Location", "http://192.168.178.1/", true);
    server.send(302, "text/plain", "");
}

void setup()
{
    pinMode(readyIndicatorPin, OUTPUT);
    digitalWrite(readyIndicatorPin, LOW);

    Serial.begin(9600);
    Serial.println();
    Serial.println("Starting ESP8266 access point...");

    // Enable AP mode
    WiFi.mode(WIFI_AP);

    // Disable WiFi sleep
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    

    // Configure AP IP
    WiFi.softAPConfig(ip, ip, nmask);

    // Start access point on channel 3
    while (!WiFi.softAP(ap_ssid, ap_password, 3))
    {
        Serial.println("ERROR: Failed to start hotspot!");
        delay(1000);
    }

    // Start wildcard DNS
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

    // HTTP routes
    server.on("/connecttest.txt", handleConnectTest);
    server.on("/ncsi.txt", handleConnectTest);
    server.on("/", HTTP_GET, handleRoot);
    server.on("/secret.html", HTTP_GET, handleSecret);

    server.onNotFound(handleNotFound);

    server.begin();

    Serial.println("HTTP server started!");

    digitalWrite(readyIndicatorPin, HIGH);
}

void loop()
{
    dnsServer.processNextRequest();
    server.handleClient();
}
