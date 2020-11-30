#include <Arduino.h>
#include <TimeLib.h>
#include <time.h>
#include "LiquidCrystal_I2C.h"
#include "BigNumbers_I2C.h"


#define blinkFreq 500 // (ms)
#define alarmBlinkFreq 1000
#define thigh 975
#define tlow 25



class digitalClock {
    public:
        void begin(void);
        void print_time(void);
        void set_time(char c, uint8_t potKnob, uint8_t confirmButton);
        void toggleBacklight(uint8_t on);
        uint8_t checkAlarm(void);
        void playAlarm(void);
        void printAlarm(void);
        void clearLCD(void);
        void printTimeOfAlarm(uint8_t h, uint8_t m, char mode);
    private:
        void check_thershold();
        void saveTime(uint8_t h, uint8_t m);
        uint8_t blinkDisplay(void);
};


