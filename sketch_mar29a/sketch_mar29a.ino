#include<LiquidCrystal.h>
#include "DHT.h"
#include <TinyGPS.h>
#include <SoftwareSerial.h>

//LCD
LiquidCrystal lcd{13, 12, 11, 10, 9, 8};

// DTH
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
char temperature[] = "Temp = 00.0 C";
char humidity[]    = "RH   = 00.0 %";

// sound
const int sound = 52;

// GAS sensor
#define MQ2Pin 24
#define MQ3Pin 25
#define MQ4Pin 22
#define MQ5Pin 26
#define MQ6Pin 27
#define MQ7Pin 23
#define MQ8Pin 28
#define MQ9Pin 29

//GPS GSM
SoftwareSerial mySerial(50, 51);
TinyGPS gps;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  mySerial.begin(9600);
  Serial1.begin(9600);
  pinMode(sound, INPUT);
  pinMode(MQ2Pin, INPUT);
  pinMode(MQ3Pin, INPUT);
  pinMode(MQ4Pin, INPUT);
  pinMode(MQ5Pin, INPUT);
  pinMode(MQ6Pin, INPUT);
  pinMode(MQ7Pin, INPUT);
  pinMode(MQ8Pin, INPUT);
  pinMode(MQ9Pin, INPUT);

  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("   Welcome To   ");
  lcd.setCursor(0,1);
  lcd.print("Healty Residence");
  delay(100);
}

void loop() {
  SendMessage("-------------M-------------");
  function_gps();
  int th = TandH();
  int s = soundSensor();
  int g = checkAirQuality();
  if(th==1 && s==0 && g==1){
    SendMessage("This location is suitable for living");
  }
  else{
    SendMessage("This location is not suitable for living");
  }
  SendMessage("---------------------------");
}

// DHT22
int TandH(){
  int val1=0, val2=0;
  lcd.clear();
  int RH = dht.readHumidity() * 10;
  int Temp = dht.readTemperature() * 10;
  
  if (isnan(RH) || isnan(Temp)) {
    lcd.setCursor(5, 0);
    lcd.print("Error");
    return 0;
  }
  if(Temp < 0){
    temperature[6] = '-';
    Temp = abs(Temp);
  }
  else
    temperature[6] = ' ';
  temperature[7]   = (Temp / 100) % 10  + 48;
  temperature[8]   = (Temp / 10)  % 10  + 48;
  temperature[10]  =  Temp % 10 + 48;
  temperature[11]  =  223;
  if(RH >= 1000)
    humidity[6]    = '1';
  else
    humidity[6]    = ' ';
  humidity[7]      = (RH / 100) % 10 + 48;
  humidity[8]      = (RH / 10) % 10 + 48;
  humidity[10]     =  RH % 10 + 48;

  //send message
  SendMessage(temperature);
  if(Temp > 220){
    SendMessage("Temperature","Hot");
    val1=0;
  }
  if(Temp < 160){
    SendMessage("Temperature","Cold");
    val1=0;
  }
  if(Temp >= 160 && Temp <= 220){
    SendMessage("Temperature","Perfect");
    val1=1;
  }
  SendMessage(humidity);
  if(RH > 500){
    SendMessage("Humidity","High Humidity");
    val2=0;
  }
  if(RH < 300){
    SendMessage("Humidity","Low Humidity");
    val2=0;
  }
  if(RH >= 300 && RH <= 500){
    SendMessage("Humidity","Perfect Humidity");
    val2=1;
  }
  lcd.setCursor(0, 0);
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print(humidity);
  delay(100);
  if(val1 == 1 && val2 == 1){
    return 1;
  }
  return 0;
}

// Sound
int soundSensor(){
  int value = digitalRead(sound);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sound Intensity:");
  if(value == 1){
    lcd.setCursor(0,1);
    lcd.print("HIGH");
    SendMessage("Sound pollution detected.");
    delay(100);
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("LOW");
    SendMessage("No sound pollution.");
    delay(100);
  }
  return value;
}
// GAS
int checkAirQuality()
{
  int gas_value2 = digitalRead(MQ2Pin);
  int gas_value3 = digitalRead(MQ3Pin);
  int gas_value4 = digitalRead(MQ4Pin);
  int gas_value5 = digitalRead(MQ5Pin);
  int gas_value6 = digitalRead(MQ6Pin);
  int gas_value7 = digitalRead(MQ7Pin);
  int gas_value8 = digitalRead(MQ8Pin);
  int gas_value9 = digitalRead(MQ9Pin);

  gas_print(gas_value2 , "MQ - 2");
  //SendMessage("Air quality pertect");
  gas_print(gas_value3 , "MQ - 3");
  gas_print(gas_value4 , "MQ - 4");
  gas_print(gas_value5 , "MQ - 5");
  gas_print(gas_value6 , "MQ - 6");
  gas_print(gas_value7 , "MQ - 7");
  gas_print(gas_value8 , "MQ - 8");
  gas_print(gas_value9 , "MQ - 9");
  
  int val=0;
  if(gas_value4==LOW && gas_value7==LOW && gas_value2==LOW && gas_value3==LOW && gas_value5==LOW && gas_value6==LOW && gas_value8==LOW && gas_value9==LOW)
  {
    SendMessage("Air quality pertect");
    val = 1;
  }
  else{
    SendMessage("Air pollution detected.");
    val=0;
  }
  return val;
}
void gas_print(int val , String st)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(st);
  lcd.setCursor(0,1);
  if(val == 1){
    lcd.print("HIGH");
  }
  else{
    lcd.print("LOW");
  }
  delay(100);
}

/// GPS
void function_gps(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }

  if (newData)      //If newData is true
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    
    //Serial.println("Polution Details of the place with: ");   
    //Serial.print("Latitude = ");
    //Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    //Serial.print(" Longitude = ");
    //Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Lati:");
    lcd.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    lcd.setCursor(0,1);
    lcd.print("Long:");
    lcd.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    SendMessage("At Location:");
    mySerial.print("Latitude : ");
    mySerial.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    mySerial.print("Longitude : ");
    mySerial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    mySerial.println((char)26);
    //SendMessage("Latitude", String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6));
    //SendMessage("Longitude", String(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6));
    delay(500);
  }
 
  Serial.println(failed);
}

// GSM
void SendMessage( String st1, String st2 )
{
  //mySerial.println("AT");

  //mySerial.println("AT+CMGF=1");

  //mySerial.println("AT+CMGS=\"+8801532990965\"\r");

  mySerial.print(st1);
  mySerial.print(" : ");
  mySerial.println(st2);
  //mySerial.println("The Area average sound is: "+st2 );

  mySerial.println((char)26);

}
void SendMessage( String st1)
{
  mySerial.println(st1);
  mySerial.println((char)26);
}
