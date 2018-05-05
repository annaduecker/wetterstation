#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_I2C_ByVac.h>
#include <LiquidCrystal_SI2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR1W.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>
#include <SI2CIO.h>
#include <SoftI2CMaster.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Wire.h";

#define COUNT_MAX 14
 
unsigned int icr1;

volatile int rx=0;
volatile int bitcount=0;
volatile char pin;
volatile unsigned char sa,sb,sd,se;
volatile unsigned int sc,sf;

//Sensor Variablen
String windDir = "";
float windSpeed = 0.0;
float temperatur;
String json;

int lcd_Data = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
 
void setup() {

 
// Initialize the digital pin as an output.
// Pin 13 has an LED connected on most Arduino boards

//Wind Setup
 
  Serial.begin(9600);
  pinMode(2, INPUT);
 
  EICRA  = _BV(ISC01);    // set INT0 to trigger on falling edge
  EIMSK = _BV(INT0);     // Turns on INT0
 
  icr1 = 19500;
 
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  OCR1A = icr1;

  //Esp8266 Setup
  //Serial.begin(9600);    // Serieller Port zeigt den Status (auf der IDE)
  Serial1.begin(115200); // Zweiter serieller Port verbunden mit ESP8266
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
  
  //LCD Setup
  lcd.begin(16,2); // sixteen characters across - 2 lines
  lcd.backlight();
  // first character - 1st line
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
  // 8th character - 2nd line 
  lcd.setCursor(8,1);
  lcd.print("-------");
  
}
 
void loop(){
  
  //Sensoren auslesen
  read_Wind();
  read_Temperatur();


  //In Json umwandeln
  json = "{";
  json += "\"Windrichtung\": ";
  json += windDir;
  json += "\r\n";
  json += "\"Windgeschwindigkeit\": ";
  json += windSpeed;
  json += "\r\n";

  json += "\"Luft-Feuchtigkeit\": "
  json += regen;
  json += "\r\n";

  

  json += "}";

  //Daten an LCD ausgeben
  lcd_printData(lcdData);
  lcd_Data ++;
  if(lcd_Data == MAX_COUNT)
  {
    lcd_Data = 0;
  }

  //Json an WLAN ausgeben
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
          wlan_show(client,json);
          break;
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

  delay(2000);
}

void read_Temperatur()
{
  temperatur = 25.3;
}

void wlan_show(WiFiEspClient client, String s)
{
   client.println(s);
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
    rx=0;
  }
 
}

void lcd_printData(int count)
{
  if(count < 2)
  {
    //Windsensor anzeigen
  }
  else if(count == 1)
  {
    //Regen anzeigen
  }
  lcd.print(""
}


//NICHT VERÄNDERN AB HIER 
// pin change interrupt ( start of frame )
ISR (INT0_vect){
  // reset accumulators
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
    Serial.print(i);
    Serial.print(",");
 
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
    bitcount--;
    if (bitcount==0){
      // all data received:
      rx=1;
    }
  } 
}
