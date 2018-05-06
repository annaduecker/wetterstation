#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <IRremote.h>   // Das Programm greift an dieser Stelle auf eine „Library“ zurück. Das erleichtert einem viel Arbeit. Denn das Infrarotlicht wird mit einem Code verschlüsselt gesendet. Um diesen Code selber auszulesen und in passende Werte umzuwandeln, wären sehr viele Zeilen Code erforderlich.
#include <WiFiEsp.h> // Header von WiFiEsp einbinden


#define SEALEVELPRESSURE_HPA (1013.25) // INIT PRESSURE FOR SENSOR

//#define ESP_RX   19
//#define ESP_TX   18
//SoftwareSerial Serial1(ESP_RX, ESP_TX); // Virtuelles RX, TX für WLAN-Modul

#define MAX_CTR 8 //even numbers
#define MAX_JSON_CTR 15 //mit 2 multipliziert = Sekunden, bevor json erneut gesendet wird
#define RAIN A0 // RAIN PIN
#define SUN A1 // SUN PIN
#define LED 13 // LED for Markise
#define PIEZO 7 // TON für Thresholds
#define SUMM 750 // TIME to 
#define RECV_PIN 11  //  Der Kontakt der am Infrarotsensor (Fernbedienung) die Daten ausgibt, wird mit Pin 11 des Arduinoboards verbunden.
#define CRITICAL_RAIN 10//80 // Threshold WIND MAX
#define RAIN_THRESHOLD_MIN 2 // Threshold RAIN MIN
#define SUN_THRESHOLD_MAX 50   // Threshold SUN  MAX
#define SUN_THRESHOLD_MIN 15// Threshold SUN  MIN
#define CRITICAL_WIND 10  // Threshold  WIND MIN
#define MIN_WIND 1.00f
#define _ESPLOGLEVEL_ 4

const byte MY_ADDRESS = 42;


bool MARKISE = false;
bool MARKISE_AUTOMATION= true;
 
//Infrarot Variablen
IRrecv irrecv(RECV_PIN);   // An dieser Stelle wird ein Objekt definiert, dass den Infrarotsensor an Pin 11 ausliest.
decode_results results;  // Dieser Befehl sorgt dafür, dass die Daten, die per Infrarot eingelesen werden unter „results“ abgespeichert werden.
//int IR_counter = 0;
bool LED_State = false; //Shows, if LED is on or off

//WLAN Variablen
#define WiFiSSID "UPC78F249A"   // Ihre SSID
#define WiFiPass "kAprQm3epjtc"   // Ihr Zugangs-Passwort
int status = WL_IDLE_STATUS;
const char server[] = "wetterstation.westeurope.cloudapp.azure.com";
WiFiEspClient client;

//Tx20 Variablen Windsensor
unsigned int icr1;
volatile int rx=0;
volatile int bitcount=0;
volatile char pin;
volatile unsigned char sa,sb,sd,se;
volatile unsigned int sc,sf;


unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds

const unsigned long postingInterval = 10000L; // delay between updates, in millisecond

//Variablen für ausgelesene Daten
String windDir = ""; // INIT for Wind direction
int windDirInt = 0; // INIT for Wind direction
float windSpeed = 0; // INIT for wind speed
/*byte sun, rain;
int temp = 0;
int pressure = 0;
int humidity = 0;
int altitude = 0;*/

//Variablen für LCD-Anzeige
byte data_ctr = 0;
Adafruit_BME280 bme; // I2C
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//Variablen für 2-Sekunden-Timer und json-sende-timer
unsigned long timer = 0;
byte sensorLoopCount = 0;

//Variable für Json-Format
String json = "";

void setup() {
// Initialize the digital pin as an output.
// Pin 13 has an LED connected on most Arduino boards
  Serial.begin(9600);
  Serial1.begin(9600); // Zweiter serieller Port verbunden mit ESP8266
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  bme.begin(0x76); 
  delay(100); //boot of sensor

  init_WLAN();

  pinMode(RAIN, INPUT);
  pinMode(SUN, INPUT);
  pinMode(LED,OUTPUT);
  pinMode(PIEZO,OUTPUT);

  irrecv.enableIRIn();   //Dieser Befehl initialisiert den Infrarotempfänger.

  timer = millis();

  delay(10000);
  
  //EICRA  = _BV(ISC00);    //Trigger on rising edge
  EICRA  = _BV(ISC01);    // set INT0 to trigger on falling edge
  
  EIMSK = _BV(INT0);     // Turns on INT0
 
  icr1 = 19500;
 
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  OCR1A = icr1;

 Wire.begin (MY_ADDRESS);
  Wire.onReceive (receiveEvent);
}

volatile boolean haveData = false;
volatile int X;
volatile float Y;

void init_WLAN()
{
  // initialize serial for ESP module
  //Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    //Serial.print("Attempting to connect to WPA SSID: ");
    //Serial.println(WiFiSSID);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WiFiSSID, WiFiPass);
  }

  // you're connected now, so print out the data
  //Serial.println("You're connected to the network");
  write_LCD(0,"WLAN connected");

  //printWifiStatus();

  //Serial.println();
  
}

void loop()
{

 if (haveData)
  {
windDir=getWindDirection(X);
 windSpeed=Y;   
 windDirInt=X;
    haveData = false;  
  }  // end if haveData


  IR_LED ();
  check_weather_critical();
  if(millis() - timer >= 2000)  //Wenn 2 Sekunden seit letztem Auslesen der Sensoren vergangen sind:
  {
    timer = millis(); //Timer zurücksetzen
    Sensor_loop();  //Sensor-Funktion ausführen
  }

  if (millis() - lastConnectionTime > postingInterval) {
    //Send Json

    create_Json();
    send_Json();
  }
}

void Sensor_loop()  //every 2 seconds
{
  /*read_rain();
  read_sun();
  read_temp();*/

 if(read_sun() >= SUN_THRESHOLD_MAX)  //Wenn sonnig (check alle 2 Sekunden, da nicht zeitkritisch)
 {
    Serial.println("SUN threshold MAX");
    LED_on_off(true,false); //LED an, Markise ausfahren
  }
  else if(read_sun() <= SUN_THRESHOLD_MIN){
    LED_on_off(false,false); //LED aus , Markise ein
    }
  
  
  lcd_printData(data_ctr++);
  if(data_ctr == MAX_CTR)
  {
    data_ctr = 0;
  }

  sensorLoopCount = sensorLoopCount % 15;

}

void receiveEvent (int howMany)
{
  if (howMany >= (sizeof X) + (sizeof Y))
  {
    I2C_readAnything (X);   
    I2C_readAnything (Y);   
    haveData = true;     
  }  
}

void check_weather_critical ()
{

  if(windSpeed >= CRITICAL_WIND || read_rain() >= CRITICAL_RAIN)
  {    
    LED_on_off(false,false);  //LED ausschalten (Markise einfahren)
  }
}

void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) {
        digitalWrite(PIEZO,HIGH);
        delayMicroseconds(period / 2);
        digitalWrite(PIEZO, LOW);
        delayMicroseconds(period / 2);
        elapsed_time += (period);
    }
}

void create_Json()
{
  //in json umwandeln
  json = "{\"data\":[{\"sensorId\": 1,\"sensorValue\":";
  json += String(read_temp());
  json += "},{\"sensorId\": 2,\"sensorValue\":";
  json += String(read_humidity());
  json += "},{\"sensorId\": 3,\"sensorValue\":";
  json += String(read_pressure());
  json += "},{\"sensorId\": 4,\"sensorValue\":";
  json += String(read_sun());
  json += "},{\"sensorId\": 5,\"sensorValue\":";
  json += String(read_rain());
  json += "},{\"sensorId\": 6,\"sensorValue\":";
  json += String(windDirInt);
  json += "},{\"sensorId\": 7,\"sensorValue\":";
  json += String(windSpeed);
  json += "},{\"sensorId\": 8,\"sensorValue\":"; 
  json += String(read_altitude());
  json += "}]}";
 Serial.println(json);
}


void IR_LED ()
{
  if (irrecv.decode(&results))  //Wenn Daten empfangen wurden,
  {    

    Serial.println(results.value);
      if(results.value == 16753245 )
      {
        playTone(250,500);
          delay(750);
        LED_State = LED_State ^ 1; // Xor -> Toggeln
        // if IR_REV is active then turn of MARKISE_AUTOMATION
        
        LED_on_off(LED_State,true);
      }
      else if( results.value == 16736925 ){
        playTone(250,500);
        delay(750);
       MARKISE_AUTOMATION = MARKISE_AUTOMATION ^ 1; // Xor -> Toggeln
       if(MARKISE_AUTOMATION){
           write_LCD(0, "Automation an");
       }
           else {
            write_LCD(0, "Automation aus");
         }
        }
    
    irrecv.resume();  //Der nächste Wert soll vom IR-Empfänger eingelesen werden
  }
}

void LED_on_off (bool statusLED,bool interupt )
{
  
  if(statusLED && !MARKISE  && (MARKISE_AUTOMATION || interupt)  && read_rain()<= RAIN_THRESHOLD_MIN && windSpeed <= MIN_WIND ) // WENN SONNE
  {
  playTone(750,750);
    write_LCD(0, "Ausgefahren");
    digitalWrite (LED, statusLED);
    MARKISE=true;
    LED_State=true;
  }
  else if(!statusLED && MARKISE)
  {
    //Serial.println("Off");
    write_LCD(0, "Eingefahren");
      digitalWrite(PIEZO,HIGH);
     playTone(750,750);
     digitalWrite (LED, statusLED);
     MARKISE=false;
     LED_State=false;
  }
 // else if(statusLED && !MARKISE_AUTOMATION ){
 //   Serial.println("Ignore SUN");
 //   }
 // delay(2000);
}

void lcd_printData(int count)
{
  if(count < 2)
  {
    //wind anzeigen
    write_LCD(0,"Winddir: "+windDir);
    write_LCD(1,"WindSpeed: "+String(windSpeed));
    Serial.print("Dir: "+windDir);
    Serial.println("\tSpeed: "+String(windSpeed));
  }
  else if(count <4)
  {
    byte rain = read_rain();
    byte sun = read_sun();
    write_LCD(0,"Rain: "+String(rain));
    write_LCD(1,"Sun: "+String(sun));
    //Serial.print("Rain: "+String(rain));
    //Serial.println("Sun: "+String(sun));
  }
  else if(count < 6)
  {
    int temp = read_temp();
    int pressure = read_pressure();
    write_LCD(0, "Temp: "+String(temp)+" C");
    write_LCD(1, "Press: "+String(pressure)+" hPa");
    //Serial.print("Temp: "+String(temp)+"C");
    //Serial.println("Press: "+String(pressure)+" hPa");
  }
  else if(count < 8)
  {
    int humidity = read_humidity();
    int altitude = read_altitude();
    write_LCD(0, "Humidity: "+String(humidity)+" %");
    write_LCD(1, "Altitude: "+String(altitude)+" m");
    //Serial.print("Humidity: "+String(humidity)+" %");
    //Serial.println("Altitude: "+String(altitude)+" m");
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

int read_temp()
{
  return bme.readTemperature();
  //pressure = bme.readPressure() / 100.0F;
  //altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  //humidity = bme.readHumidity();
}

int read_pressure()
{
  return bme.readPressure() / 100.0F;
}

int read_altitude()
{
  return bme.readAltitude(SEALEVELPRESSURE_HPA);
}

int read_humidity()
{
  return bme.readHumidity();
}

byte read_rain(){
  return map(analogRead(RAIN),0,1023,0,100);
}

byte read_sun(){
  return map(analogRead(SUN),0,1023,0,100);
}

String getWindDirection(int sb){
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
      return windDir;
    }
  

void send_Json()
{
  client.stop();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 3000)) {
   
    // Make a HTTP request
    client.println("POST /weatherStationServer/sensordata/add HTTP/1.1");
    client.println("Host: wetterstation.westeurope.cloudapp.azure.com");
    client.println("Accept: */*");
    client.println("Content-Length: " + String(json.length()));
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(json);
     lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");

  }
}
///////////////////////////////////////////////////////////
//  Methode sendHttpResponse():
//     Methode, mit der wir dem Client eine Webseite senden
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Methode printWifiStatus(): 
//    Ausgabe WiFi-Status und Info im seriellen Monitor
///////////////////////////////////////////////////////

/*void printWifiStatus()
{
  // Netzwerkdaten: SSID
  //Serial.print("SSID: ");
  //Serial.println(WiFi.SSID());

  // Netzwerkdaten: IP-Adresse
  //IPAddress ip = WiFi.localIP();
  //Serial.print("IP Adresse: ");
  //Serial.println(ip);

  // Hinweis im seriellen Monitor
  //Serial.println();
  //Serial.print("Um den Fernzugriff zu testen, gehen Sie bitte auf die Webseite http://");
  //Serial.println(ip);
  //Serial.println();
}*/

 
// pin change interrupt ( start of frame )
ISR (INT0_vect){
  // reset accumulators

  Serial.println("Interrupt");
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
    Serial.println("Clearing");
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

template <typename T> unsigned int I2C_writeAnything (const T& value)
  {
  Wire.write((byte *) &value, sizeof (value));
  return sizeof (value);
  }  // end of I2C_writeAnything

template <typename T> unsigned int I2C_readAnything(T& value)
  {
    byte * p = (byte*) &value;
    unsigned int i;
    for (i = 0; i < sizeof value; i++)
          *p++ = Wire.read();
    return i;
  }  // end of I2C_readAnything
