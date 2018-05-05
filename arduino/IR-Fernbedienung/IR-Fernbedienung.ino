#include <IRremote.h>   // Das Programm greift an dieser Stelle auf eine „Library“ zurück. Das erleichtert einem viel Arbeit. Denn das Infrarotlicht wird mit einem Code verschlüsselt gesendet. Um diesen Code selber auszulesen und in passende Werte umzuwandeln, wären sehr viele Zeilen Code erforderlich.

#define LED 13
#define RECV_PIN 11  //  Der Kontakt der am Infrarotsensor die Daten ausgibt, wird mit Pin 11 des Arduinoboards verbunden.

IRrecv irrecv(RECV_PIN);   // An dieser Stelle wird ein Objekt definiert, dass den Infrarotsensor an Pin 11 ausliest.

decode_results results;  // Dieser Befehl sorgt dafür, dass die Daten, die per Infrarot eingelesen werden unter „results“ abgespeichert werden.
int IR_counter = 0;

bool LED_State = false;

void setup()
{

Serial.begin(9600);    //Im Setup wird die Serielle Verbindung gestartet, damit man sich die Empfangenen Daten der Fernbedienung per seriellen Monitor ansehen kann.

pinMode (LED, OUTPUT);

irrecv.enableIRIn();   //Dieser Befehl initialisiert den Infrarotempfänger.

}


void loop()

{   

if (irrecv.decode(&results)) {    //Wenn Daten empfangen wurden,
  if(IR_counter++ % 2 == 0)
  {
    if(results.value == 16753245)
    {
      LED_State ^= 1;
      digitalWrite(LED, LED_State);
      if(LED_State)
      {
        Serial.println("On");
      }
      else
      {
        Serial.println("Off");
      }
    }
  }


irrecv.resume();  //Der nächste Wert soll vom IR-Empfänger eingelesen werden
}
}
