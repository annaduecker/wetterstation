#include <WiFiEsp.h> // Header von WiFiEsp einbinden
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// Entweder SoftSerial (UNO, LEONARDO) auf Pins 6+7 oder 
// Hardware-Serial (RX1,TX1) (MEGA, DUE)

/*#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6,7); // Virtuelles RX, TX
#endif
*/
char WiFiSSID[] = "AndroidAP";   // Ihre SSID
char WiFiPass[] = "anjy1450 ";   // Ihr Zugangs-Passwort
int status = WL_IDLE_STATUS;


// LED: Initialzustand und Pin
int led    = LOW;
int ledPIN = 10;  //LED an Pin 10

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
  //Serial.begin(9600);    // Serieller Port zeigt den Status (auf der IDE)
  Serial.begin(115200); // Zweiter serieller Port verbunden mit ESP8266
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.clear();
  write_LCD(0,"Hello");
  WiFi.init(&Serial);   // Initialisierung des Ports

  // Hat es geklappt?
  if (WiFi.status() == WL_NO_SHIELD) {
    write_LCD(0,"ESP8266 nicht gefunden");
    // Wenn nein, dann in Endlosschleife gehen
    while (0 == 0);
  }

  // Verbindung initiieren
  while (status != WL_CONNECTED) {
    write_LCD(0,"Verbindungsaufbau zu  ");
    write_LCD(0,WiFiSSID);
    // Verbindung zum WPA-Netzwerk aufbauen
    status = WiFi.begin(WiFiSSID, WiFiPass);
  }

  write_LCD(0,"Verbindung erfolgreich");
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
    write_LCD(0,"Neue Client-Verbindung"); // Ausgabe auf seriellen  
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
          write_LCD(0,"LED wird eingeschaltet");
          led = HIGH;
          digitalWrite(ledPIN, HIGH);
        }
        else if (puffer.endsWith("GET /AUS")) {
          write_LCD(0,"LED wird ausgeschaltet");
          led = LOW;
          digitalWrite(ledPIN, LOW);
        }
      }

      else{
        write_LCD(0,"No data available");
      }
    }
    
    // Verbindung schließen
    client.stop();
    write_LCD(0,"Clientverbindung wurde geschlossen");
  }
}

void write_LCD(int line, String s)
{
  //lcd.clear();
  lcd.setCursor(0,line);
  lcd.print(s);
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
  write_LCD(0,"SSID: ");
  write_LCD(0,WiFi.SSID());

  // Netzwerkdaten: IP-Adresse
  IPAddress ip = WiFi.localIP();
  write_LCD(0,"IP Adresse: ");
  write_LCD(1,String(ip));

  // Hinweis im seriellen Monitor
  write_LCD(0,"Um den Fernzugriff zu testen, gehen Sie bitte auf die Webseite http://");
  write_LCD(0,String(ip));
}
