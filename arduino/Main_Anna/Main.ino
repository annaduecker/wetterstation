#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define MAX_CTR 8 //even numbers
#define RAIN A0
#define SUN A1
#define LED 13
 
unsigned int icr1;
volatile int rx=0;
volatile int bitcount=0;
volatile char pin;
volatile unsigned char sa,sb,sd,se;
volatile unsigned int sc,sf;

//Variablen für ausgelesene Daten
String windDir = "";
float windSpeed = 0.0;
int sun, rain;
float temp = 0.0;
float pressure = 0.0;
float humidity = 0.0;
float altitude = 0.0;

int data_ctr = 0;

Adafruit_BME280 bme; // I2C
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

void setup() {
 
// Initialize the digital pin as an output.
// Pin 13 has an LED connected on most Arduino boards
  Serial.begin(9600);
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  bme.begin(0x76); 
  delay(100); //boot of sensor
  
  pinMode(2, INPUT);  //Wind
  pinMode(RAIN, INPUT);
  pinMode(SUN, INPUT);
  pinMode(LED,OUTPUT);

  //EICRA  = _BV(ISC00);    //Trigger on rising edge
  EICRA  = _BV(ISC01);    // set INT0 to trigger on falling edge
  
  EIMSK = _BV(INT0);     // Turns on INT0
 
  icr1 = 19500;
 
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  OCR1A = icr1;
}

void loop()
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

  delay(2000);

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
  else if(count < 4)
  {
    write_LCD(0,"Rain: "+String(rain));
    write_LCD(1,"Sun: "+String(sun));
    Serial.print("Rain: "+String(rain));
    Serial.println("Sun: "+String(sun));
  }
  else if(count < 6)
  {
    int t = temp;
    int p = pressure;
    write_LCD(0, "Temp: "+String(t)+" C");
    write_LCD(1, "Press: "+String(p)+" hPa");
    Serial.print("Temp: "+String(t)+"C");
    Serial.println("Press: "+String(p)+" hPa");
  }
  else if(count < 8)
  {
    int h = humidity;
    int alt = altitude;
    write_LCD(0, "Humidity: "+String(h)+" %");
    write_LCD(1, "Altitude: "+String(alt)+" m");
    Serial.print("Humidity: "+String(h)+" %");
    Serial.println("Altitude: "+String(alt)+" m");
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
      Serial.print("Shit received:");
      sprintf (a,"%d\t%d\n",sb,sc);
      Serial.write (a);
    }
    rx=0;
  }
 
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
