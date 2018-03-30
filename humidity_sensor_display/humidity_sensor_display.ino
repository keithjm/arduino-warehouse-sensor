#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>
#include <dht.h>
#include "DHT.h"

//SCA goes to A4
//SCL goes to A5

#define dht_apin A0 // Analog Pin sensor is connected to

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

dht DHT;

int decreasePin = 2;
int increasePin = 3;
int selectPin = 4;

int humiditySet = 50;
int tempSet = 50;

int downButtonState = 0;
int upButtonState = 0;
int selectButtonState = 0;

unsigned long previousMillisButtonPress = 0;
unsigned long previousMillisHumidityReading = 0;

int intervalHumidityReading = 5000;
int intervalButtonPress = 250;
int selectMode = 0;

 
void setup(){
  pinMode(decreasePin, INPUT);
  pinMode(increasePin, INPUT);
  pinMode(selectPin, INPUT);
  lcd.begin(16,2);
  lcd.clear();
  Serial.begin(9600);
  delay(500);//Delay to let system boot
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(1000);//Wait before accessing Sensor
}

 
void loop(){
  unsigned long currentMillis = millis();
  
  if((unsigned long)(currentMillis - previousMillisHumidityReading) >= intervalHumidityReading) {
    readHumidity(); 
    
    previousMillisHumidityReading = currentMillis;
  }
  if((unsigned long)(currentMillis - previousMillisButtonPress) >= intervalButtonPress){
    buttoncontrol();
    relayControl();
    previousMillisButtonPress = currentMillis;
  }
  
}

void buttoncontrol() {
  downButtonState = digitalRead(decreasePin);
  upButtonState = digitalRead(increasePin);
  selectButtonState = digitalRead(selectPin);
  
  if(selectButtonState == HIGH) {
    Serial.println("Select Button Pressed\n\n");

    selectMode++;
    if (selectMode > 2) {
      selectMode = 0;
    }
  }

  if(selectMode == 0) {
    printHumidityTemp();
  }
  
  if(selectMode == 1){
      lcd.clear();                    
      lcd.print("Set Humidity: ");
      lcd.setCursor ( 0, 1 );
      lcd.print(humiditySet);
      lcd.print("%");
        
      if (downButtonState == HIGH) {
        //downButtonState = !downButtonState;
        setHumidity(false);
      }
      if (upButtonState == HIGH) {
        //upButtonState = !upButtonState;
        setHumidity(true);
      }
  }
  if(selectMode == 2){

      lcd.clear();                    
      lcd.print("Set Temp: ");
      lcd.setCursor ( 0, 1 );
      lcd.print(tempSet);
      lcd.print((char)223);
      lcd.print("F");
      
      if (downButtonState == HIGH) {
        //downButtonState = !downButtonState;
        Serial.println("Down Button Pressed\n\n");
        setTemp(false);
      }
      if (upButtonState == HIGH) {
        Serial.println("Up Button Pressed\n\n");
        //upButtonState = !upButtonState;
        setTemp(true);
      }
  }
}

void readHumidity(){
  
    DHT.read11(dht_apin);
    
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");

}

void printHumidityTemp(){
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(round(DHT.temperature * 1.8 + 32));
    lcd.print((char)223);
    lcd.print("F");
    lcd.setCursor ( 0, 1 );
    lcd.print("Humidity: ");
    lcd.print(round(DHT.humidity));
    lcd.print("%");
    lcd.home();
}

void setHumidity(boolean directionChange) {
  if(!directionChange) {
    lcd.clear();                    
    lcd.print("Set Humidity: ");
    lcd.setCursor ( 0, 1 );
    lcd.print(humiditySet);
    lcd.print("%");
    if(humiditySet > 0){
      humiditySet --;
    }
  } else {
    lcd.clear();                    
    lcd.print("Set Humidity: ");
    lcd.setCursor ( 0, 1 );
    lcd.print(humiditySet);
    lcd.print("%");
    if(humiditySet < 100) {
      humiditySet ++;
    }
  }
}

void setTemp(boolean directionChange){
  
  if(directionChange) {
    lcd.clear();                    
    lcd.print("Set Temp: ");
    lcd.setCursor ( 0, 1 );
    lcd.print(tempSet);
    lcd.print((char)223);
    lcd.print("F");
    if(tempSet < 100) {
      tempSet ++;
    }
  } else {
    lcd.clear();                    
    lcd.print("Set Temp: ");
    lcd.setCursor ( 0, 1 );
    lcd.print(tempSet);
    lcd.print((char)223);
    lcd.print("F");
    if(tempSet > -10) {
      tempSet --;
    }
  }
}

void relayControl(){
  if(round(DHT.temperature * 1.8 + 32) >= tempSet && DHT.humidity >= humiditySet){
    if(selectMode == 0){
      lcd.setCursor(14, 0);
      lcd.print("ON");
    }
  } else {
    if(selectMode == 0) {
      lcd.setCursor(13, 0);
      lcd.print("OFF");
    }
  }
 
}



