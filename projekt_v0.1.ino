#include <OneWire.h>
#include <DS18B20.h>

#define ONEWIRE_PIN 2

#include <SoftwareSerial.h>// import the serial library

#define MIN_VOLTAGE     600 // mv - próg dolnego zakresu napięcia dla braku pyłu
#define VREF           1100 // mv - napięcie referencyjne komparatora
#define PIN_LED           7 // numer pinu ILED
#define PIN_ANALOG        0 // numer pinu AOUT
#define MAX_ITERS        150 // liczba pomiarow do sredniej


SoftwareSerial Genotronex(10, 11); // RX, TX
int ledpin=13; // led on D13 will show blink on / off
int BluetoothData; // the data given from Computer

int ADC_VALUE; // odczytana wartosc A0
int ITER; // numer pomiaru
float VOLTAGE; // wartosc napiecia
float DUST; // wynik
float AVG_DUST; // sredni wynik

float temperature;

byte address[8]= {0x28, 0x9D, 0x16, 0x1F, 0x0, 0x0, 0x80, 0x2B};

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);

void setup() {

  analogReference(INTERNAL);
  // inicjalizacja czujnikow
  // inne wazne gówna
  while(!Serial);
  Serial.begin(9600);
  sensors.begin();
  sensors.request(address);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

Genotronex.begin(9600);
  Genotronex.println("Bluetooth On please press 1 or 0 blink LED ..");
  pinMode(ledpin,OUTPUT);
    
}

float computeDust()
{
  // Blyskamy IR, czekamy 280ms, odczytujemy napiecie ADC
  digitalWrite(PIN_LED, HIGH);
  delayMicroseconds(280);
  ADC_VALUE = analogRead(PIN_ANALOG);
  digitalWrite(PIN_LED, LOW);
 
  // Przeliczamy na mV. Calosc mnozymy przez 11, poniewaz w module
  // zastosowano dzielinik napiecia 1k/10k
  VOLTAGE = (VREF / 1024.0) * ADC_VALUE * 11;
 
  // Obliczamy zanieczyszczenie jesli zmierzone napiecie ponad prog
 
  if (VOLTAGE > MIN_VOLTAGE)
  {
    return (VOLTAGE - MIN_VOLTAGE) * 0.2;
  }
 
  return 0;
}

void loop() {
  if (sensors.available()){
    temperature = sensors.readTemperature(address);

    //Serial.print(temperature);
    //Serial.println(F(" 'C"));

    sensors.request(address);
  }
  if(temperature>-10 && temperature<65){
  AVG_DUST = 0;
   ITER = 0;
 
   while (ITER < MAX_ITERS)
   {
     DUST = computeDust();
     // Do sredniej liczmy tylko prawidlowe pomiary
     if (DUST > 0)
     {
       AVG_DUST += DUST;
       ITER++;
       delay(50);
     }     
   }
   
   AVG_DUST /= MAX_ITERS;
   
   Serial.print("D = ");
   Serial.print(AVG_DUST);
   Serial.println("ug/m3");    
  if (Genotronex.available()){
  Genotronex.print("D = ");
   Genotronex.print(AVG_DUST);
   Genotronex.println("ug/m3"); 
}    
   delay(500);

     /*if(BluetoothData=='1'){   // if number 1 pressed ....
   digitalWrite(ledpin,1);
   Genotronex.println("LED  On D13 ON ! ");
   }
  if (BluetoothData=='0'){// if number 0 pressed ....
  digitalWrite(ledpin,0);
   Genotronex.println("LED  On D13 Off ! ");
  }*/


  }
  /*
  onewire.reset_search();
  while(onewire.search(address)){
    if (address[0] != 0x28)
    continue;
    if(OneWire::crc8(address, 7) != address[7]){
      Serial.println(F("Bledny adres"));
      break;
    }
      for (byte i=0;i<8;i++){
      Serial.print(F("0x"));
      Serial.print(address[i], HEX);

      if (i<7)
      Serial. print(F(", "));
    }
    Serial.println();
    }
    while(1);*/
    }

  // if(temperatura>-10 && temperatura < 65){
  // dsm501{
  /*
   * for(i=0;i<200;i++){
   * pomiar dsm+= pomiar chwilowy
   * }
   * pomiar wlasciwy = pomiar dsm/200
   * }
  }*/
  // bluetooth
  // wysyłanie po bluetooth
  // dalsze plany:
  // -gps
  // -karta sd
  // -wysyłanie po wifi

