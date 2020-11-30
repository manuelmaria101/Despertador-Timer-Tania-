#include <Arduino.h>
#include <TimeLib.h>
#include <time.h>
#include <Talkie.h>
#include "PCM.h"
#include "LiquidCrystal_I2C.h"
#include "BigNumbers_I2C.h"
#include "DigitalClock.h"

#define set_timer A1
#define confirm_button 8
#define lights 7
#define RedLights 6
#define COLDLIGHT 60
#define OFFLIGHT 10
#define SEC2 2000 //2 sec
#define SEC3 3000 //3 sec
#define MS500 500// 0.5 sec
#define MS750 750 //0,75 sec

digitalClock myClock;
uint8_t s = 0, stateLight = 1, alarmIsSet = 0, stateLEDS = 1;
unsigned long before = 0;



void setup() {
  myClock.begin();
  Serial.begin(115200);
  pinMode(set_timer, INPUT);
  pinMode(confirm_button, INPUT);
  pinMode(lights, OUTPUT);
  pinMode(RedLights, OUTPUT);
  digitalWrite(RedLights, LOW);
  digitalWrite(lights, HIGH);
  myClock.set_time('T', set_timer, confirm_button);
}

void loop(){
    if(alarmIsSet < 2)
      myClock.print_time();
    else {
      myClock.playAlarm();
      delay(50);
      myClock.printAlarm();
    }

    if(digitalRead(confirm_button) ==  HIGH && s == 0 && alarmIsSet < 2){
      s = 1;
      before = millis();
    }
    else if(digitalRead(confirm_button) == LOW && s == 1 && alarmIsSet < 2){ //carregando uma vez no botao vamos para alterar novamente as horas
      s = 3;
      before = millis();
    }
    else if(s == 1 && (millis() -  before) >= SEC2){ //permite ligar/desligar a luz do LCD se pressionado o botao durante +2sec
      s = 2;
      if(stateLight){
        myClock.toggleBacklight(0);
        stateLight = 0;
      }
      else {
        myClock.toggleBacklight(1);
        stateLight = 1;
      }
    }
    else if(s == 2 && digitalRead(confirm_button) == LOW && alarmIsSet < 2) s = 0;
    else if(s == 2 && digitalRead(confirm_button) == HIGH && alarmIsSet < 2 && (millis() - before) >= SEC3){
      if(stateLEDS){
        digitalWrite(lights, LOW);
        digitalWrite(RedLights, LOW);
        stateLEDS = 0;
      }
      else {
        digitalWrite(lights, HIGH);
        digitalWrite(RedLights, HIGH);
        stateLEDS = 1;
      }
      s = 5;
    }
    else if(s == 5 && digitalRead(confirm_button) == LOW && alarmIsSet < 2) s = 0;
    else if(s == 3 && (millis() - before) <= MS500 && digitalRead(confirm_button) == HIGH && alarmIsSet < 2){ //duplo clique para programar o alarme
      s = 4;
      before = millis();
    }
    else if(s == 4 && alarmIsSet == 1 && digitalRead(confirm_button) == HIGH && (millis() - before) > MS750){
      while(digitalRead(confirm_button) == HIGH)
        myClock.printTimeOfAlarm(NULL, NULL, 'M');
      s = 0;
    }
    else if(s == 4 && digitalRead(confirm_button) == LOW && (millis() - before) <= MS750 ){
      myClock.set_time('A', set_timer, confirm_button);
      alarmIsSet = 1;
      delay(50);
      s = 0;
    }
    else if(s == 3 && (millis() - before) > MS500){ // se o duplo clique nao for rapido conta so como 1 click e vamos mudar a hora
      myClock.set_time('T', set_timer, confirm_button);
      delay(50);
      s = 0;
    }

    if(alarmIsSet == 1 && myClock.checkAlarm()){
      myClock.playAlarm();
      myClock.clearLCD();
      delay(75);
      myClock.printAlarm();
      myClock.toggleBacklight(1);
      digitalWrite(lights, HIGH);
      digitalWrite(RedLights, HIGH);
      alarmIsSet = 2;
    }
    else if(alarmIsSet == 2 && digitalRead(confirm_button) == HIGH){
      uint16_t pot = map(analogRead(set_timer), 0, 1024, 0, 100);
      if(pot < 25){
        alarmIsSet = 3;
      }
    }
    else if( alarmIsSet == 3 && map(analogRead(set_timer), 0, 1024, 0, 100) > 75){
      alarmIsSet = 0;
      myClock.clearLCD();
    }

    if(stateLEDS == 1 && map(analogRead(set_timer), 0, 1024, 0, 100) < COLDLIGHT && map(analogRead(set_timer), 0, 1024, 0, 100) > OFFLIGHT){
      digitalWrite(lights, LOW);
      digitalWrite(RedLights, HIGH);
    }
    else if(stateLEDS == 1 && map(analogRead(set_timer), 0, 1024, 0, 100) >= COLDLIGHT){
      digitalWrite(lights, HIGH);
      digitalWrite(RedLights, LOW);
    }
    else if(stateLEDS == 1 && map(analogRead(set_timer), 0, 1024, 0, 100) <= OFFLIGHT){
      digitalWrite(lights, LOW);
      digitalWrite(RedLights, LOW);
    }
}