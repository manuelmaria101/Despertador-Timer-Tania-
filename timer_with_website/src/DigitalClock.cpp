#include <Arduino.h>
#include <TimeLib.h>
#include <time.h>
#include "PCM.h"
#include "LiquidCrystal_I2C.h"
#include "BigNumbers_I2C.h"
#include "DigitalClock.h"
#include "sounds.h"




unsigned long prevblink, prevAlarmBlink;
LiquidCrystal_I2C lcd(0x27, 16, 2); 
BigNumbers_I2C bigNum(&lcd);
uint8_t alarmBlink = 0, blink = 0, alarm_hour = 255, alarm_min = 255;

void digitalClock::begin(void){
    lcd.begin();
    bigNum.begin();
    lcd.clear();
}

void digitalClock::print_time(){
    bigNum.displayLargeInt(hour(), 1, 0, 2, 1);
    if(!blinkDisplay()){
        lcd.setCursor(7, 0);
        lcd.print(":");
    }
    else if(blinkDisplay()){
        lcd.setCursor(7, 0);
        lcd.print(" ");
    }
    bigNum.displayLargeInt(minute(), 8, 0, 2, 1);
}

void digitalClock::printTimeOfAlarm(uint8_t h, uint8_t m, char mode){
    switch (mode)
    {
        case 'S' : //Set time of alarm
            bigNum.displayLargeInt(h, 1, 0, 2, 1);
            lcd.setCursor(7, 0);
            lcd.print(":");
            bigNum.displayLargeInt(m, 8, 0, 2, 1);
        break;

        case 'M' : //Show time of alarm (M = mostrar)
            bigNum.displayLargeInt(alarm_hour, 1, 0, 2, 1);
            lcd.setCursor(7,0);
            lcd.print(":");
            bigNum.displayLargeInt(alarm_min, 8, 0, 2, 1);
    default:
        break;
    }
}

void digitalClock::set_time(char c, uint8_t potKnob, uint8_t confirmButton){
    uint8_t h, x = 0;
    int16_t pot = 0;
    lcd.clear();
    while(1){
        pot = map(analogRead(potKnob), tlow, thigh, 0, 23);
        delay(50);
        //set up hour
        if(pot >= 23) pot = 23;
        else if(pot <= 0) pot = 0;
        if(c == 'T'){
            setTime(pot, 0, 0, 1, 1, 2020);
            print_time();
        }
        else if(c == 'A'){
            saveTime(pot, NULL);
            printTimeOfAlarm(pot, 0, 'S');
        }
        if(digitalRead(confirmButton) == HIGH && x == 0)
            x = 1;  
        else if(digitalRead(confirmButton) == LOW && x == 1){
            x = 2;
            h = pot;
            while(1){
                pot = map(analogRead(potKnob), tlow, thigh, 0, 59);
                delay(50);
                //set up minute
                if(pot >= 59) pot = 59;
                else if(pot <= 0) pot = 0;  
                if(c == 'T'){
                    setTime(h, pot, 0, 1, 1, 2020);
                    print_time();
                }
                else if(c == 'A'){
                    saveTime(NULL, pot);
                    printTimeOfAlarm(h, pot, 'S');
                }
                if(digitalRead(confirmButton) == HIGH && x == 2)
                    x = 3;
                else if(digitalRead(confirmButton) == LOW && x ==3) break;
            }
            break;
        }
    }
    delay(100);
}

void digitalClock::saveTime(uint8_t h, uint8_t m){
    if(h!= NULL) alarm_hour = h;
    else if(m !=NULL) alarm_min = m;
}

void digitalClock::toggleBacklight(uint8_t on){
    if(on) lcd.backlight();
    else lcd.noBacklight();
}

uint8_t digitalClock::checkAlarm(void){
    if(alarm_hour == hour() && alarm_min == minute()){
        return 1;
    }
    return 0;
}

uint8_t digitalClock::blinkDisplay(void){
    int aux = millis()- prevblink;
    if(blink == 0 && aux >= blinkFreq){
        blink = 1;
        prevblink = millis();
    }
    else if(blink == 1 && aux >=blinkFreq){
        blink = 0;
        prevblink = millis();
    }
    return blink;
}


unsigned long prev1;
uint8_t aux1 = 0;

void digitalClock::playAlarm(void){
    if(aux1 == 0 || (aux1 == 3 && (millis()- prev1)>= TIMETANAHORA)){
        startPlayback(BOMDIA, sizeof(BOMDIA));
        aux1 = 1;
        prev1 = millis();
    }
    else if(aux1 == 1 && (millis() - prev1)>=TIMEBOMDIA){
        startPlayback(TANIA, sizeof(TANIA));
        aux1 = 2;
        prev1 = millis();
    }
    else if(aux1 == 2 && (millis()- prev1) >= TIMETANIA){
        startPlayback(TANAHORA, sizeof(TANAHORA));
        aux1 = 3;
        prev1 = millis();
    }
}

void digitalClock::printAlarm(void){
    int aux = millis()- prevAlarmBlink;
    if(alarmBlink == 0 && aux >= alarmBlinkFreq){
        // lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("ALERTA ALARME");
        lcd.setCursor(6,1);
        if(hour() < 10)
            lcd.setCursor(7,1);
        lcd.print(hour());
        lcd.setCursor(9,1);
        if(minute() < 10){
            lcd.print("0");
            lcd.setCursor(10, 1);
        }
        lcd.print(minute());
        alarmBlink = 1;
        prevAlarmBlink = millis();
    }
    else if(alarmBlink == 1 && aux >=alarmBlinkFreq){
        alarmBlink = 0;
        lcd.clear();
        lcd.setCursor(6,1);
        if(hour() < 10)
            lcd.setCursor(7,1);
        lcd.print(hour());
        lcd.setCursor(9,1);
        if(minute() < 10){
            lcd.print("0");
            lcd.setCursor(10, 1);
        }
        lcd.print(minute());
        prevAlarmBlink = millis();
    }

    if(!blinkDisplay()){
        lcd.setCursor(8, 1);
        lcd.print(":");
    }
    else if( blinkDisplay()){
        lcd.setCursor(8, 1);
        lcd.print(" ");
    }
}


void digitalClock::clearLCD(void){
    lcd.clear();
}