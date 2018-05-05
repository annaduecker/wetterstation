/*
 WiFiEsp example: WebClient
 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.
 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "KarosWLAN";   // Ihre SSID
char pass[] = "saqq7515";   // Ihr Zugangs-Passwort
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "wetterstation.westeurope.cloudapp.azure.com";

// Initialize the Ethernet client object
WiFiEspClient client;

String json;
String key;

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 3000)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    json = "{\"data\":[{\"sensorId\": 1,\"sensorValue\":7},{\"sensorId\": 2,\"sensorValue\":6},{\"sensorId\": 3,\"sensorValue\":5},{\"sensorId\": 4,\"sensorValue\":4},{\"sensorId\": 5,\"sensorValue\":3},{\"sensorId\": 6,\"sensorValue\":2},{\"sensorId\": 7,\"sensorValue\":1}]}";
    client.println("POST /weatherStationServer/sensordata/add HTTP/1.1");
    client.println("Host: wetterstation.westeurope.cloudapp.azure.com");
    client.println("Accept: */*");
    client.println("Content-Length: " + String(json.length()));
    client.println("Content-Type: application/json");
    client.println();
    client.println(json);


  }
  Serial.println("No Data yet");
  while(!client.available());
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}

void create_Json()
{
  //In Json umwandeln
  json = "{";
  json += "\"data\":[{";
  json += "\"sensorId\":" +String(1) + ",\"sensorValue\":";
  json += String(7);  //Sensor 1
  json += "},";
  json += "{\"sensorId\":" +String(2) + ",\"sensorValue\":";
  json += String(6);  //Sensor 2
  json += "},";
  json += "{\"sensorId\":" +String(3) + ",\"sensorValue\":";
  json += String(5); //Sensor 3
  json += "},";
  json += "{\"sensorId\":" +String(4) + ",\"sensorValue\":";
  json += String(4);  //Sensor 4
  json += "},";
  json += "{\"sensorId\":" +String(5) + ",\"sensorValue\":";
  json += String(3); //Sensor 5
  json += "},";
  json += "{\"sensorId\":" +String(6) + ",\"sensorValue\":";
  json += String(2); //Sensor 6
  json += "},";
  json += "{\"sensorId\":" +String(7) + ",\"sensorValue\":";
  json += String(1); //Sensor 7

  //Noch nicht in Json enthalten
  /*
  json += "},";
  json += "{\"sensorId\": \"8\",\"sensorValue\":";
  json += altitude;
  */
  json += "}]}";
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
