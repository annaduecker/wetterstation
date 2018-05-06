#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>  // Comes with Arduino IDE
//#include <LiquidCrystal_I2C.h>

#define _MDEBUG

unsigned int icr1;
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

  const byte SLAVE_ADDRESS = 42;



void setup() {

  // Initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards
#ifdef _MDEBUG
  pinMode(13, OUTPUT);
#endif

  Serial.begin(9600);
  // lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  pinMode(2, INPUT);

  //EICRA  = _BV(ISC00);    //Trigger on rising edge
  EICRA  = _BV(ISC01);    // set INT0 to trigger on falling edge

  EIMSK = _BV(INT0);     // Turns on INT0

  icr1 = 19500;

  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  OCR1A = icr1;

  // init communication
 Wire.begin ();

}
volatile int rx = 0;
volatile int bitcount = 0;
volatile char pin;
volatile unsigned char sa, sb, sd, se;
volatile unsigned int sc, sf;

int windDir = "";
float windSpeed = 0.0;

void loop()
{
  read_wind();

  Serial.print("WindDir: ");
  Serial.print(windDir);
  Serial.print("\tWindSpeed: ");
  Serial.println(windSpeed);

  // write_LCD(0,"Dir: "+windDir);

  String sp = String(windSpeed);
  //write_LCD(1,"Speed: "+sp);
   //your code here

        char *X = malloc(1000 * sizeof(windSpeed)); // use dynamic number instead of 1000

  float Y =windSpeed;

  Wire.beginTransmission (SLAVE_ADDRESS);
  I2C_writeAnything (windDir);
  I2C_writeAnything (Y);
  Wire.endTransmission ();

   delay(2000);

}



/*
  void write_LCD(int line, String s)
  {
  //lcd.clear();
  lcd.setCursor(0,line);
  lcd.print(s);
  }
*/

void read_wind() {
  char a[90];
  unsigned char chk;

  // Main code loop
  // TODO: Put your regular (non-ISR) logic here

  if (rx) {
    chk = ( sb + (sc & 0xf) + ((sc >> 4) & 0xf) + ((sc >> 8) & 0xf) ); chk &= 0xf;
    if (sa == 4 && sb == se && sc == sf && sd == chk) {
      //sprintf (a,"%d\t%d\n",sb,sc);
      //Serial.write (a);

      windDir=sb;
      windSpeed = sc / 10.0;
   
    }
    else
    {
      Serial.print("Shit received:");
      sprintf (a, "%d\t%d\n", sb, sc);
      Serial.write (a);
    }
    rx = 0;
  }

}

// pin change interrupt ( start of frame )
ISR (INT0_vect) {
  // reset accumulators

  //Serial.println("Interrupt");
  sa = sb = sd = se = 0;
  sc = 0; sf = 0;

  EIMSK &= ~_BV(INT0);     // disable pin interrupt

  bitcount = 41;

  TCCR1B = 0; // stop timer
  TCNT1 = icr1 >> 1; // set counter
  TIFR1 |= _BV(OCF1A); // clear timer interrupt
  TCCR1B = _BV(WGM12) | _BV(CS10); // start timer
  TIMSK1 |= _BV(OCIE1A);  // enable timer interrupt

}

// timer irq ( bit accumulator )
ISR(TIMER1_COMPA_vect) {

  if (bitcount == 1) {
    TIMSK1 &= ~_BV(OCIE1A); // disable timer interrupt
    EIFR |= _BV(INTF0);     // clear pin interrupt flag
    EIMSK = _BV(INT0);      // enable pin interrupt flag for next frame

  }
  if (bitcount > 0) {
    pin = digitalRead(2);
    int i = pin;
    //Serial.print(i);
    //Serial.print(",");

    if (bitcount > 41 - 5) {
      // start
      sa = (sa << 1) | pin;
    } else if (bitcount > 41 - 5 - 4) {
      // wind dir
      sb = sb >> 1 | (pin << 3);
    } else if (bitcount > 41 - 5 - 4 - 12) {
      // windspeed
      sc = sc >> 1 | (pin << 11);
    } else if (bitcount > 41 - 5 - 4 - 12 - 4) {
      // checksum
      sd = sd >> 1 | (pin << 3);
    } else if (bitcount > 41 - 5 - 4 - 12 - 4 - 4) {
      // wind dir, inverted
      se = se >> 1 | ((pin ^ 1) << 3);
    } else {
      // windspeed, inverted
      sf = sf >> 1 | ((pin ^ 1) << 11);
    }


    //digitalWrite( 13, digitalRead( 13 ) ^ 1 );
#ifdef _MDEBUG
    digitalWrite( 13, 0 );
    digitalWrite( 13, 1 );
    digitalWrite( 13, 1 );
    digitalWrite( 13, 0 );
#endif
    bitcount--;
    if (bitcount == 0) {
      // all data received:
      rx = 1;
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
