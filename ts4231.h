/*******************************************************************
    Copyright (C) 2017 Triad Semiconductor

    ts4231.h - Library for configuring the Triad Semiconductor TS4231 Light
               to Digital converter.
    Created by: John Seibel
*******************************************************************/

#ifndef ts4231_h
#define ts4231_h

#include <stdint.h>

#define BUS_DRV_DLY     1       //delay in microseconds between bus level changes
#define BUS_CHECK_DLY   500     //delay in microseconds for the checkBus() function
#define SLEEP_RECOVERY  100     //delay in microseconds for analog wake-up after exiting SLEEP mode
#define UNKNOWN_STATE   0x04    //checkBus() function state
#define S3_STATE        0x03    //checkBus() function state
#define WATCH_STATE     0x02    //checkBus() function state
#define SLEEP_STATE     0x01    //checkBus() function state
#define S0_STATE        0x00    //checkBus() function state
#define CFG_WORD        0x392B  //configuration value
#define BUS_FAIL        0x01    //configDevice() function status return value
#define VERIFY_FAIL     0x02    //configDevice() function status return value
#define WATCH_FAIL      0x03    //configDevice() function status return value
#define CONFIG_PASS     0x04    //configDevice() function status return value

class TS4231 {

  public:
    TS4231(int device_E_pin, int device_D_pin);
    bool waitForLight(uint16_t light_timeout);  //timeout in milliseconds
    bool goToSleep(void);
    uint8_t configDevice(uint16_t config_val = CFG_WORD);
    bool goToWatch(void);

  private:
    uint8_t checkBus(void);
    void ts_delayUs(unsigned int delay_val);  //delay in microseconds
    void ts_pinMode(int pin, uint8_t mode);
    uint8_t ts_digitalRead(int pin);
    void ts_digitalWrite(int pin, uint8_t write_val);
    unsigned long ts_millis(void);
    void writeConfig(uint16_t config_val);
    uint16_t readConfig(void);
    int E_pin;
    int D_pin;
    bool configured;
};    

#endif
