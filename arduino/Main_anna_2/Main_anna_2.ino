#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <IRremote.h>   // Das Programm greift an dieser Stelle auf eine „Library“ zurück. Das erleichtert einem viel Arbeit. Denn das Infrarotlicht wird mit einem Code verschlüsselt gesendet. Um diesen Code selber auszulesen und in passende Werte umzuwandeln, wären sehr viele Zeilen Code erforderlich.
#include <WiFiEsp.h> // Header von WiFiEsp einbinden
#include "SoftwareSerial.h"

#define SEALEVELPRESSURE_HPA (1013.25)

SoftwareSerial Serial1(6,7); // Virtuelles RX, TX für WLAN-Modul

#define MAX_CTR 8 //even numbers
#define RAIN A0
#define SUN A1
#define LED 13
#define RECV_PIN 11  //  Der Kontakt der am Infrarotsensor die Daten ausgibt, wird mit Pin 11 des Arduinoboards verbunden.

//Infrarot Variablen
IRrecv irrecv(RECV_PIN);   // An dieser Stelle wird ein Objekt definiert, dass den Infrarotsensor an Pin 11 ausliest.
decode_results results;  // Dieser Befehl sorgt dafür, dass die Daten, die per Infrarot eingelesen werden unter „results“ abgespeichert werden.
int IR_counter = 0;
bool LED_State = false; //Shows, if LED is on or off

//WLAN Variablen
char WiFiSSID[] = "KarosWLAN";   // Ihre SSID
char WiFiPass[] = "saqq7515";   // Ihr Zugangs-Passwort
int status = WL_IDLE_STATUS;
WiFiEspServer WebServer(80); // Vereinbarung des Mini-WebServers 
RingBuffer puffer(24);  // in WiFiESP ist ein Ringpuffer-Typ vereinbart, den wir nutzen

//Tx20 Variablen
unsigned int icr1;
volatile int rx=0;
volatile int bitcount=0;
volatile char pin;
volatile unsigned char sa,sb,sd,se;
volatile unsigned int sc,sf;

//Variablen für ausgelesene Daten
String windDir = "";
int windSpeed = 0;
int sun, rain;
int temp = 0;
int pressure = 0;
int humidity = 0;
int altitude = 0;

//Variablen für LCD-Anzeige
int data_ctr = 0;
Adafruit_BME280 bme; // I2C
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//Variable für 2-Sekunden-Timer
unsigned long timer = 0;

//Variable für Json-Format
String json = "";

void setup() {
// Initialize the digital pin as an output.
// Pin 13 has an LED connected on most Arduino boards
  //Serial.begin(9600);
  Serial1.begin(9600); // Zweiter serieller Port verbunden mit ESP8266
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  bme.begin(0x76); 
  delay(100); //boot of sensor

  init_WLAN();
  
  pinMode(2, INPUT);  //Wind
  pinMode(RAIN, INPUT);
  pinMode(SUN, INPUT);
  pinMode(LED,OUTPUT);

  irrecv.enableIRIn();   //Dieser Befehl initialisiert den Infrarotempfänger.

  //EICRA  = _BV(ISC00);    //Trigger on rising edge
  EICRA  = _BV(ISC01);    // set INT0 to trigger on falling edge
  
  EIMSK = _BV(INT0);     // Turns on INT0
 
  icr1 = 19500;
 
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  OCR1A = icr1;

  timer = millis();
}

void init_WLAN()
{
  WiFi.init(&Serial1);   // Initialisierung des Ports
  // Hat Wifi-Init geklappt?
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("ESP8266 nicht gefunden");
    // Wenn nein, dann in Endlosschleife gehen
    while (0 == 0);
  }
  // Verbindung initiieren
  while (status != WL_CONNECTED) {
    //Serial.print("Verbindungsaufbau zu  ");
    //Serial.println(WiFiSSID);
    status = WiFi.begin(WiFiSSID, WiFiPass);// Verbindung zum WPA-Netzwerk aufbauen
  }
  //Serial.println("Verbindung erfolgreich");
  printWifiStatus();
  WebServer.begin(); // Webserver am Port 80 aktivieren
}

void loop()
{
  WLAN_loop();
  IR_LED ();
  if(millis() - timer >= 2000)  //Wenn 2 Sekunden seit letztem Auslesen der Sensoren vergangen sind:
  {
    timer = millis(); //Timer zurücksetzen
    Sensor_loop();  //Sensor-Funktion ausführen
  }
}


void Sensor_loop()  //every 2 seconds
{
  
  read_wind();
  read_rain();
  read_sun();
  read_temp();

  lcd_printData(data_ctr++);
  if(data_ctr == MAX_CTR)
  {
    data_ctr = 0;
  }

  create_Json();

}

void check_weather ()
{
  
}

void create_Json()
{
  //In Json umwandeln
  json = "{";
  json += "\"Windrichtung\": ";
  json += windDir;
  json += "\r\n";
  json += "\"Windgeschwindigkeit\": ";
  json += windSpeed;
  json += "\r\n";
  json += "\"Regen\": ";
  json += rain;
  json += "\r\n";
  json += "\"Sonne: \"";
  json += sun;
  json += "\r\n";
  json += "\"Temperatur: \"";
  json += temp;
  json += "\r\n";
  json += "\"Luft-Druck: \"";
  json += pressure;
  json += "\r\n";
  json += "\"Luft-Feuchtigkeit: \"";
  json += humidity;
  json += "\r\n";
  json += "\"Hoehe: \"";
  json += altitude;
  json += "\r\n";
  json += "}";
}


void IR_LED ()
{
  if (irrecv.decode(&results))  //Wenn Daten empfangen wurden,
  {    
    if(IR_counter++ % 2 == 0)
    {
      if(results.value == 16753245)
      {
        LED_State = LED_State ^ 1; // Xor -> Toggeln
        LED_on_off(LED_State);
      }
    }
    irrecv.resume();  //Der nächste Wert soll vom IR-Empfänger eingelesen werden
  }
}


void LED_on_off (bool on)
{
  digitalWrite (LED, on);
  if(on)
  {
    //Serial.println("On");
    write_LCD(0, "Ausgefahren");
  }
  else
  {
    //Serial.println("Off");
    write_LCD(0, "Eingefahren");
  }
  delay(2000);
}


void lcd_printData(int count)
{
  if(count < 2)
  {
    //wind anzeigen
    write_LCD(0,"Winddir: "+windDir);
    write_LCD(1,"WindSpeed: "+String(windSpeed));
    //Serial.print("Dir: "+windDir);
    //Serial.println("\tSpeed: "+String(windSpeed));
  }
  else if(count < 4)
  {
    write_LCD(0,"Rain: "+String(rain));
    write_LCD(1,"Sun: "+String(sun));
    //Serial.print("Rain: "+String(rain));
    //Serial.println("Sun: "+String(sun));
  }
  else if(count < 6)
  {
    int t = temp;
    int p = pressure;
    write_LCD(0, "Temp: "+String(t)+" C");
    write_LCD(1, "Press: "+String(p)+" hPa");
    //Serial.print("Temp: "+String(t)+"C");
    //Serial.println("Press: "+String(p)+" hPa");
  }
  else if(count < 8)
  {
    int h = humidity;
    int alt = altitude;
    write_LCD(0, "Humidity: "+String(h)+" %");
    write_LCD(1, "Altitude: "+String(alt)+" m");
    //Serial.print("Humidity: "+String(h)+" %");
    //Serial.println("Altitude: "+String(alt)+" m");
  }
}


void write_LCD(int line, String s)
{
  if(line == 0)
  {
    lcd.clear(); 
  }
  lcd.setCursor(0,line);
  lcd.print(s);
}

void read_temp()
{
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
}

void read_rain(){
  rain = analogRead(RAIN);
}

void read_sun(){
  sun = analogRead(SUN);
}
 
void read_wind(){
  char a[90];
  unsigned char chk;
 
  // Main code loop
  // TODO: Put your regular (non-ISR) logic here
 
  if (rx){
    chk= ( sb + (sc&0xf) + ((sc>>4)&0xf) + ((sc>>8)&0xf) );chk&=0xf;
    if (sa==4 && sb==se && sc==sf && sd==chk){      
      //sprintf (a,"%d\t%d\n",sb,sc);
      //Serial.write (a);
      
      switch(sb)
      {
        case 0: windDir = "N"; break;
        case 1: windDir = "NNE"; break;
        case 2: windDir = "NE"; break;
        case 3: windDir = "ENE"; break;
        case 4: windDir = "E"; break;
        case 5: windDir = "ESE"; break;
        case 6: windDir = "SE"; break;
        case 7: windDir = "SSE"; break;
        case 8: windDir = "S"; break;
        case 9: windDir = "SSW"; break;
        case 10: windDir = "SW"; break;
        case 11: windDir = "WSW"; break;
        case 12: windDir = "W"; break;
        case 13: windDir = "WNW"; break;
        case 14: windDir = "NW"; break;
        case 15: windDir = "NNW"; break;
      }
      windSpeed = sc/10.0;
    }
    else
    {
      //Serial.print("Shit received:");
      sprintf (a,"%d\t%d\n",sb,sc);
      Serial.write (a);
    }
    rx=0;
  }
 
}

void WLAN_loop()
{
  // Warten auf Verbindungswünsche von Clients
  WiFiEspClient client = WebServer.available(); 

  if (client) {                               // Client klopft an.
    //Serial.println("Neue Client-Verbindung"); // Ausgabe auf seriellen  
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
      }

      else{
        //Serial.println("No data available");
      }
    }
    
    // Verbindung schließen
    client.stop();
    //Serial.println("Clientverbindung wurde geschlossen");
  }
}
///////////////////////////////////////////////////////////
//  Methode sendHttpResponse():
//     Methode, mit der wir dem Client eine Webseite senden
///////////////////////////////////////////////////////////

void sendHttpResponse(WiFiEspClient client)
{
  client.print(json);
}

///////////////////////////////////////////////////////
// Methode printWifiStatus(): 
//    Ausgabe WiFi-Status und Info im seriellen Monitor
///////////////////////////////////////////////////////

void printWifiStatus()
{
  // Netzwerkdaten: SSID
  //Serial.print("SSID: ");
  //Serial.println(WiFi.SSID());

  // Netzwerkdaten: IP-Adresse
  IPAddress ip = WiFi.localIP();
  //Serial.print("IP Adresse: ");
  //Serial.println(ip);

  // Hinweis im seriellen Monitor
  //Serial.println();
  //Serial.print("Um den Fernzugriff zu testen, gehen Sie bitte auf die Webseite http://");
  //Serial.println(ip);
  //Serial.println();
}

 
// pin change interrupt ( start of frame )
ISR (INT0_vect){
  // reset accumulators

  //Serial.println("Interrupt");
  sa=sb=sd=se=0;
  sc=0;sf=0;
 
  EIMSK &= ~_BV(INT0);     // disable pin interrupt
 
  bitcount=41;
 
  TCCR1B = 0; // stop timer
  TCNT1 = icr1>>1; // set counter
  TIFR1 |= _BV(OCF1A); // clear timer interrupt
  TCCR1B = _BV(WGM12)| _BV(CS10); // start timer
  TIMSK1 |= _BV(OCIE1A);  // enable timer interrupt
 
}
 
// timer irq ( bit accumulator )
ISR(TIMER1_COMPA_vect){
 
  if (bitcount==1){
    TIMSK1 &= ~_BV(OCIE1A); // disable timer interrupt
    EIFR |= _BV(INTF0);     // clear pin interrupt flag
    EIMSK = _BV(INT0);      // enable pin interrupt flag for next frame
 
  }
  if (bitcount>0){
    pin = digitalRead(2);
    int i = pin;
    //Serial.print(i);
    //Serial.print(",");
 
    if (bitcount > 41-5){
      // start
      sa = (sa<<1)|pin;
    } else
    if (bitcount > 41-5-4){
      // wind dir
      sb = sb>>1 | (pin<<3);
    } else
    if (bitcount > 41-5-4-12){
      // windspeed
      sc = sc>>1 | (pin<<11);
    } else
    if (bitcount > 41-5-4-12-4){
      // checksum
      sd = sd>>1 | (pin<<3);
    } else 
    if (bitcount > 41-5-4-12-4-4){
      // wind dir, inverted
      se = se>>1 | ((pin^1)<<3);
    } else {
      // windspeed, inverted
      sf = sf>>1 | ((pin^1)<<11);
    }     
 
 
//digitalWrite( 13, digitalRead( 13 ) ^ 1 );
    bitcount--;
    if (bitcount==0){
      // all data received:
      rx=1;
    }
  } 
}
