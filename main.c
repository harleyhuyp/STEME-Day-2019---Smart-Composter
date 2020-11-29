/* Terminated on July 12 2019 by Huy P */
/* 
   Code was written in C 
   Please remember to download some libraries.
   Use Arduino IDE
*/
/* NLTNC = Number(s) in This Line Need(s) to be Changed (NTLNC)*/
#include <Servo.h> //My lungs are black, my heart is pure
#include <Wire.h>; //My hands are scarred from nights before

#include <SoftwareSerial.h> //And my hair is thinnin', falling out
SoftwareSerial nodemcu(2,3);//Of all the wrong places
String cdata;               //I am a little insecure

#include "RTClib.h"
RTC_DS1307 RTC;

#include <OneWire.h>
#include <DallasTemperature.h> 
#define ONE_WIRE_BUS A1
OneWire oneWire(A1);
DallasTemperature sensors(&oneWire);

#include <DHT.h>
DHT dht(A3, DHT11);

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#include <HX711.h>
HX711 scale;

Servo myservo1;
Servo myservo2;

const unsigned int  SM     = A0; //Soil Moisture sensor
const unsigned char IR     = A2;

const unsigned char IN1    = 4; //may bom khi
const unsigned char IN2    = 7; //may tron
const unsigned char IN3    = 8; //dien tro nhiet
const unsigned char IN4    = 10;

const unsigned char Servo1 = 6;  //nap
const unsigned char Servo2 = 5;  //yeast

const unsigned char WPM    = 13; // Water Pump Motor

unsigned short weight,preweight;
bool inside;

void gate (bool p)
  {
    if (p) {myservo1.write(100); delay(900);}
      else {myservo1.write(0); delay(300);}
  }
void yeast()
  {
    myservo2.write(90); delay(500);   //NTLNC
    myservo2.write(0);
  }
void water(bool v) // bom nuoc
{
  if (v == true)
  {digitalWrite(WPM, HIGH);}
  else {digitalWrite(WPM, LOW);}
}
unsigned int temperature()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
void heat(bool b)
  {
    if (b) {digitalWrite(IN3,LOW);}
    else {digitalWrite(13,HIGH); digitalWrite(9,HIGH); digitalWrite(IN1,LOW);}
  }
void setup()
{
  Serial.begin(9600);
  nodemcu.begin(9600);
  sensors.begin();
  //lcd.begin();
  
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  
  //lcd.init();
  lcd.backlight();

  dht.begin();
    
  myservo1.attach(Servo1);
  myservo2.attach(Servo2);
 
  scale.begin(12, 11);
  scale.set_scale(2280.f);
  scale.tare();

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  
  preweight = scale.get_units(10);;
  inside = false;

  Wire.begin();
    Wire.beginTransmission(0x68);
    Wire.write(0x07);
    Wire.write(0x10);
    Wire.endTransmission();
  RTC.begin();
    if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(_DATE_, _TIME_));
 }
}  
void loop()
{
    if ((analogRead(IR)) < 90) {gate(true);}
    else gate(false);
    DateTime now = RTC.now();
    
    unsigned char intihours, intidays, intimonths, days, hours;
      intihours  = (now.hour(), DEC);
      intidays   = (now.day(), DEC);
      
              unsigned char soilmoisture;
              soilmoisture = analogRead(SM);
              soilmoisture = map(soilmoisture, 1023, 0, 0, 100);
              soilmoisture = constrain(soilmoisture, 0, 100);
              sensors.requestTemperatures();
              unsigned char soiltemperature;
              soiltemperature = sensors.getTempCByIndex(0);
              soiltemperature = map(soiltemperature, 0, 1023, -50, 200);
              soiltemperature = constrain(soiltemperature, -50, 200);

    if ((weight - preweight) > 500)
      {
        inside = true;
        yeast();
        while (soilmoisture < 90)
          {
            water(true);
          }
        water(false);
        
      while (temperature() > 68)
        {
          heat(true);
        }
      while (temperature() < 20)
        {
          heat(false);
        }
      }
    if (inside)
        {
          weight = scale.get_units(10);

          lcd.setCursor(0, 0);
          lcd.print("AT: "); lcd.print(dht.readTemperature()); lcd.print(" ST:"); lcd.print(temperature()); lcd.print(" H:"); lcd.print(dht.readHumidity());
          lcd.setCursor(0,1);
      
          days  = (now.day(), DEC) - intidays;
          hours = (now.hour(), DEC) - intihours;
          lcd.print("SM "); lcd.print(soilmoisture); lcd.print("%, "); lcd.print(days); lcd.print(" D ,"); lcd.print(hours); lcd.print(" H");

          cdata = cdata +dht.readHumidity() +","+soilmoisture+","+dht.readTemperature()+","+temperature()+","+weight+","+ days+","+hours;
          nodemcu.println(cdata);
          cdata = "";
        } 
}
