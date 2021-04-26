/*
Drew Thomas
11/17/20
EE151 Bland
Alarm Clock Final Project
 */
 
#include "DHT.h"
#include <LiquidCrystal.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>
#define DHTTYPE DHT11   // DHT 11

const int DHT_PIN = 13; // define pin for sensor
const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 7, d7 = 6; // define pins for LCD
const int receiverpin = 10; // reciever pin for IR sensor
const int tonePin = 11; // pin to passive buzzer
int setAlarmTime = 1200; // initialize global alarm variables
int alarmState100 = 12; // reading for hour place of alarm
int alarmState1 = 0; // reading for minute place of alarm
int h=12; // hours init at 12:00 AM
int m; // minutes
int s; // seconds
int flag; // flag to change AM to PM
int actualTime; // conversion for display time to readable time
//volatile int setTemp = 75; // starting set temperature

DHT dht(DHT_PIN, DHTTYPE); // initialize dht sensor
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // initialize LCD 
IRrecv irrecv(receiverpin); // Create an irrecv object instance for IRremote 
decode_results results; // Create a place to hold the IR code read
void setup() {
  pinMode(DHT_PIN, INPUT); // setting pin modes for input and output pins
  pinMode(receiverpin, INPUT_PULLUP);
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  irrecv.enableIRIn(); // Start the IR receiver
  lcd.setCursor(0, 1);
  lcd.print("Alarm: ");
  displayTimeAlarm();
//  attachInterrupt(digitalPinToInterrupt(BUTTON1), increaseSetpoint, LOW); // set up interrupt 
//  attachInterrupt(digitalPinToInterrupt(BUTTON2), decreaseSetpoint, LOW);
}

void translateIR(){ // takes action based on IR code received
  switch (results.value){
   case 0xFF22DD: // left fast forward
    decreaseSetAlarm(100); // decrease hour
    break;
   case 0xFFC23D: // right fast forward
    increaseSetAlarm(100); // increase hour
    break;
   case 0xFFE01F: // down button
    decreaseSetAlarm(1); // decrease minute
    break;
   case 0xFF906F: // up button
    increaseSetAlarm(1); // increase minute
    break;
   case 0xFFFFFFFF: Serial.println("REPEAT"); break; // error messge for repeat
   default: Serial.println("Invalid Code"); break;  // error message for wrong button
 }
 delay(100); // check every .1 sec
}
void increaseSetAlarm(int ammt){ // increase alarm time 
  if(ammt > 59){ // hours
    setAlarmTime+=100;
    alarmState100+=1; 
  }else if(ammt < 59){ // minutes
    setAlarmTime+=ammt;
    alarmState1+=1;
  }
  displayTimeAlarm(); // display set alarm on lcd
}

void decreaseSetAlarm(int ammt){ // decrease alarm time
  if(ammt > 59){ // hours
    setAlarmTime-=100;
    alarmState100-=1;
  }else if(ammt < 59){ // minutes
    setAlarmTime-=ammt;
    alarmState1-=1;
  }
  displayTimeAlarm(); // display set alarm time on lcd
}

void displayTimeAlarm(){ // function to display on lcd
  int dispAlarmTime;
  lcd.setCursor(7, 1);
  if(alarmState100 <= 0){  
    dispAlarmTime = setAlarmTime + 1200;
    if(alarmState1 <= 59 && alarmState1 >= 0){ // if exceeds bottom limit
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else if(alarmState1 < 0){
      setAlarmTime-=40;
      alarmState1 = 59;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else{
      setAlarmTime+=40;
      alarmState1 = 0;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }
  }else if(alarmState100 < 12 && alarmState100 > 0){ // between midnight and noon
    if(alarmState1 <= 59 && alarmState1 >= 0){
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else if(alarmState1 < 0){
      setAlarmTime-=40;
      alarmState1 = 59;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else{
      setAlarmTime+=40;
      alarmState1 = 0;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }
  }else if(alarmState100 >= 12 && alarmState100 < 24){  // between noon and midnight
    if(alarmState1 <= 59 && alarmState1 >= 0){
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else if(alarmState1 < 0){
      setAlarmTime-=40;
      alarmState1 = 59;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }else{
      setAlarmTime+=40;
      alarmState1 = 0;
      lcd.print(setAlarmTime);
      lcd.println(" AM");
    }
  }else if(alarmState100 >= 24){ // if exceeds top limit
    setAlarmTime-=2400;
  }
}
void alarm(){  // function to turn on relay and buzzer idealy
  // set off alarm
}

void loop() {
  if (irrecv.decode(&results)){ // Have we received an IR signal?
    translateIR(); // Determine which key pressed
    irrecv.resume(); // Get ready to receive the next value
  }
  actualTime = 100*h + m; // get actual time to compare to set alarm time
  lcd.setCursor(0,0); 
  s=s+1; // increment second every 1000 ms
  lcd.print("Time:");
  lcd.print(actualTime); // display time

  if(flag<12){ // morning
    lcd.setCursor(9, 0);
    lcd.print("AM");
  }
  if(flag==12){ // noon
    lcd.setCursor(9, 0);
    lcd.print("PM");
  }
  if(flag>12){ // afternoon
    lcd.setCursor(9, 0);
    lcd.print("PM");
  }
  if(flag==24){ // reset at midnight
    flag=0;
  }

  delay(1000); // wait 1 sec to incriment time
//  lcd.clear();
  if(s==60){ // increase min
    s=0;
    m=m+1;
  }
  if(m==60){ // increase hour
    m=0;
    h=h+1;
    flag=flag+1;
  }
  if(h==13){ // convert from military to standard time
    h=1;
  }

  if(actualTime == setAlarmTime){ // if current time equals alarm time execute alarm function
    alarm();
  }

}
