#include <WiFiEsp.h> // Header von WiFiEsp einbinden

// Entweder SoftSerial (UNO, LEONARDO) auf Pins 6+7 oder 
// Hardware-Serial (RX1,TX1) (MEGA, DUE)

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6,7); // Virtuelles RX, TX
#endif

char WiFiSSID[] = "KarosWLAN";   // Ihre SSID
char WiFiPass[] = "saqq7515";   // Ihr Zugangs-Passwort
int status = WL_IDLE_STATUS;

// LED: Initialzustand und Pin
int led    = LOW;
int ledPIN = 13;  //LED an Pin 10

WiFiEspServer WebServer(80); // Vereinbarung des Mini-WebServers  

// in WiFiESP ist ein Ringpuffer-Typ vereinbart, den wir nutzen
RingBuffer puffer(24);

 //////////////////////////////////////////////////////////////////////
//    setup(): 
//        Initialisieren der beiden seriellen Ports, Anmeldung am WLAN
//        und Start des Webservers 
//////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);    // Serieller Port zeigt den Status (auf der IDE)
  Serial1.begin(9600); // Zweiter serieller Port verbunden mit ESP8266
  WiFi.init(&Serial1);   // Initialisierung des Ports

  // Hat es geklappt?
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("ESP8266 nicht gefunden");
    // Wenn nein, dann in Endlosschleife gehen
    while (0 == 0);
  }

  // Verbindung initiieren
  while (status != WL_CONNECTED) {
    Serial.print("Verbindungsaufbau zu  ");
    Serial.println(WiFiSSID);
    // Verbindung zum WPA-Netzwerk aufbauen
    status = WiFi.begin(WiFiSSID, WiFiPass);
  }

  Serial.println("Verbindung erfolgreich");
  printWifiStatus();
  
  // Webserver am Port 80 aktivieren
  WebServer.begin();
}

/////////////////////////////////////////////////////////////////
// loop()-Methode:
//    In der Endlosschleife warten wir auf eingehende 
//    Clientverbindungen, kommunizieren mit dem jeweiligen Client
//    und schließen die Verbindung wieder (Session-Ende!)
/////////////////////////////////////////////////////////////////

void loop()
{
  // Warten auf Verbindungswünsche von Clients
  WiFiEspClient client = WebServer.available(); 

  if (client) {                               // Client klopft an.
    Serial.println("Neue Client-Verbindung"); // Ausgabe auf seriellen  
                                              // Monitor 
    puffer.init();                            // Ringpuffer initialisieren
    while (client.connected()) {              // Solange Client verbunden
      if (client.available()) {               // Wenn Client was sendet,
        char c = client.read();               // wird das Byte gelesen und 
        puffer.push(c);                       // im Puffer abgelegt
                                  
        
        // Ein HTTP Reauest endet mit zweimal Zeilenvorsprung/Wagenrücklauf
        // => Clientrequest gelesen => Daten senden
        if (puffer.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }

        // Endung der Anfrage: http://<webaddresse>/AN oder .../AUS?
        if (puffer.endsWith("GET /AN")) {
          Serial.println("LED wird eingeschaltet");
          led = HIGH;
          digitalWrite(ledPIN, HIGH);
        }
        else if (puffer.endsWith("GET /AUS")) {
          Serial.println("LED wird ausgeschaltet");
          led = LOW;
          digitalWrite(ledPIN, LOW);
        }
      }

      else{
        Serial.println("No data available");
      }
    }
    
    // Verbindung schließen
    client.stop();
    Serial.println("Clientverbindung wurde geschlossen");
  }
}
///////////////////////////////////////////////////////////
//  Methode sendHttpResponse():
//     Methode, mit der wir dem Client eine Webseite senden
///////////////////////////////////////////////////////////

void sendHttpResponse(WiFiEspClient client)
{
  // Hier geben wir eine HTTP-Antwort im vorgeschriebenen Format zurück. 
  // Erst der Header...
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // Und dann der Inhalt:
  client.println("<H1> LED-Steuerung am Arduino </h1> <br>");
  client.print("Im Augenblick ist die LED ");
  if (led == LOW) client.print(" <b>ausgeschaltet</b>.");
  else client.print("<b>eingeschaltet</b>.");
  client.println("<br>");
  client.println("<br>");
  client.println("Klicken Sie <a href=\"/AN\">hier</a>, um die LED einzuschalten<br>");
  client.println("Klicken Sie <a href=\"/AUS\">hier</a>, um die LED auszuschalten<br>");
  
  // Und am Schluss weitere Leerzeile einfügen
  client.println();
}

///////////////////////////////////////////////////////
// Methode printWifiStatus(): 
//    Ausgabe WiFi-Status und Info im seriellen Monitor
///////////////////////////////////////////////////////

void printWifiStatus()
{
  // Netzwerkdaten: SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Netzwerkdaten: IP-Adresse
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Adresse: ");
  Serial.println(ip);

  // Hinweis im seriellen Monitor
  Serial.println();
  Serial.print("Um den Fernzugriff zu testen, gehen Sie bitte auf die Webseite http://");
  Serial.println(ip);
  Serial.println();
}
