/*******************************************************************
    Copyright (C) 2017 Triad Semiconductor

    ts4231.h - Library for configuring the Triad Semiconductor TS4231 Light
               to Digital converter.
    Created by: John Seibel
*******************************************************************/

#include "ts4231.h"
#include <Arduino.h>

//IMPORTANT NOTES:
//1)  If porting the TS4231 library code to a non-Arduino architecture,
//    be sure that the INPUT ports assigned to the E and D signals are configured as
//    floating inputs with NO pull-up or pull-down function.  Using a pull-up or
//    pull-down function on the inputs will cause the TS4231 to operate incorrectly.
//2)  The TS4231 library omits delays between E and D signal transitions when going
//    from S3_STATE to WATCH_STATE or SLEEP_STATE to WATCH_STATE in function
//    goToWatch() for the purpose of transitioning into WATCH_STATE as quickly as
//    possible.  If a microcontroller is being used that can change states on
//    the E and D outputs faster than approximately 100ns, the TS4231 datasheet
//    must be consulted to verify timing parameters are not being violated to
//    assure proper TS4231 operation.  A suitable solution would be to include
//    a short delay in function ts_digitalWrite() to allow enough time between
//    output pin signal changes to meet the TS4231 timing parameters as stated
//    in the datasheet.  See the ts_digitalWrite() function for more information.

TS4231::TS4231(int device_E_pin, int device_D_pin) {
  configured = false;
  E_pin = device_E_pin;
  D_pin = device_D_pin;
  ts_pinMode(E_pin, INPUT);
  ts_pinMode(D_pin, INPUT);
  }

void TS4231::ts_delayUs(unsigned int delay_val) {
  delayMicroseconds(delay_val);
  }

void TS4231::ts_pinMode(int pin, uint8_t mode) {
  pinMode(pin, mode);
  }

uint8_t TS4231::ts_digitalRead(int pin) {
  uint8_t read_val;
  
  read_val = digitalRead(pin);
  return read_val;
  }

void TS4231::ts_digitalWrite(int pin, uint8_t write_val) {
  digitalWrite(pin, write_val);
//A short delay function can be inserted here to extend the time between writes to
//the E and D outputs if TS4231 timing parameters are being violated.  Consult
//the TS4231 datasheet for more information on timing parameters.  It is recommended
//that any added delay be no longer than approximately 1us.
  }

unsigned long TS4231::ts_millis() {
  unsigned long current_time;
  
  current_time = millis();
  return current_time;
  }

//Function waitForLight() should be executed after power-up and prior to
//configuring the device.  Upon power-up, D is a 0 and will output a 1
//when light is detected.  D will return to 0 at the end of light detection.
//This funciton looks for the falling edge of D to indicate that the end of
//light detection has occurred.
bool TS4231::waitForLight(uint16_t light_timeout) {
  bool light = false;
  bool exit = false;
  unsigned long time0;

  if (checkBus() == S0_STATE) {
    time0 = ts_millis();
    while (exit == false) {
      if (ts_digitalRead(D_pin) > 0) {
        while (exit == false) {
          if (ts_digitalRead(D_pin) == 0) {
            exit = true;
            light = true;
            }
          else if (ts_millis() > (time0 + light_timeout)) {
            exit = true;
            light = false;
            }
          else {
            exit = false;
            light = false;
            }
          }
        }
      else if (ts_millis() > (time0 + light_timeout)) {
        exit = true;
        light = false;
        }
      else {
        exit = false;
        light = false;
        }
      }
    }
  else light = true;  //if not in state S0_state, light has already been detected
  return light;
  }

bool TS4231::goToSleep(void) {
  bool sleep_success;
  
  if (configured == false)  sleep_success = false;
  else {
    switch (checkBus()) {
      case S0_STATE:
        sleep_success = false;
        break;
      case SLEEP_STATE:
        sleep_success = true;
        break;
      case WATCH_STATE:
        ts_digitalWrite(E_pin, LOW);
        ts_pinMode(E_pin, OUTPUT);
        ts_delayUs(BUS_DRV_DLY);
        ts_pinMode(E_pin, INPUT);
        ts_delayUs(BUS_DRV_DLY);
        if (checkBus() == SLEEP_STATE) sleep_success = true;
        else sleep_success = false;
        break;
      case S3_STATE:
        sleep_success = false;
        break;
      default:
        sleep_success = false;
        break;
      }
    }
  return sleep_success;
  }


uint8_t TS4231::configDevice(uint16_t config_val) {
  uint8_t config_success = 0x00;
  uint16_t readback;
  
  configured = false;
  ts_pinMode(D_pin, INPUT);
  ts_pinMode(E_pin, INPUT);
  ts_digitalWrite(D_pin, LOW);
  ts_digitalWrite(E_pin, LOW);
  ts_pinMode(E_pin, OUTPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_pinMode(D_pin, OUTPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_pinMode(E_pin, INPUT);
  ts_pinMode(D_pin, INPUT);
  if (checkBus() == S3_STATE) {
    writeConfig(config_val);
    readback = readConfig();
    if (readback == config_val) {
      configured = true;
      if (goToWatch()) config_success = CONFIG_PASS;
      else config_success = WATCH_FAIL;
      }
    else config_success = VERIFY_FAIL;
    }
  else config_success = BUS_FAIL;
  
  return config_success;
  }

void TS4231::writeConfig(uint16_t config_val) {
  ts_digitalWrite(E_pin, HIGH);
  ts_digitalWrite(D_pin, HIGH);
  ts_pinMode(E_pin, OUTPUT);
  ts_pinMode(D_pin, OUTPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  for (uint8_t i = 0; i < 15; i++) {
    config_val = config_val << 1;
    if ((config_val & 0x8000) > 0) ts_digitalWrite(D_pin, HIGH);
    else ts_digitalWrite(D_pin, LOW);
    ts_delayUs(BUS_DRV_DLY);
    ts_digitalWrite(E_pin, HIGH);
    ts_delayUs(BUS_DRV_DLY);
    ts_digitalWrite(E_pin, LOW);
    ts_delayUs(BUS_DRV_DLY);
    }
  ts_digitalWrite(D_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_pinMode(E_pin, INPUT);
  ts_pinMode(D_pin, INPUT);
  }

uint16_t TS4231::readConfig(void) {
  uint16_t readback;
  
  readback = 0x0000;
  ts_digitalWrite(E_pin, HIGH);
  ts_digitalWrite(D_pin, HIGH);
  ts_pinMode(E_pin, OUTPUT);
  ts_pinMode(D_pin, OUTPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_pinMode(D_pin, INPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, LOW);
  ts_delayUs(BUS_DRV_DLY);
  for (uint8_t i = 0; i < 14; i++) {
    ts_digitalWrite(E_pin, HIGH);
    ts_delayUs(BUS_DRV_DLY);
    readback = (readback << 1) | (ts_digitalRead(D_pin) & 0x0001);
    ts_digitalWrite(E_pin, LOW);
    ts_delayUs(BUS_DRV_DLY);
    }
  ts_digitalWrite(D_pin, LOW);
  ts_pinMode(D_pin, OUTPUT);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(E_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_digitalWrite(D_pin, HIGH);
  ts_delayUs(BUS_DRV_DLY);
  ts_pinMode(E_pin, INPUT);
  ts_pinMode(D_pin, INPUT);
  return readback;
  }

//checkBus() performs a voting function where the bus is sampled 3 times
//to find 2 identical results.  This is necessary since light detection is
//asynchronous and can indicate a false state.
uint8_t TS4231::checkBus(void) {
  uint8_t state;
  uint8_t E_state;
  uint8_t D_state;
  uint8_t S0_count = 0;
  uint8_t SLEEP_count = 0;
  uint8_t WATCH_count = 0;
  uint8_t S3_count = 0;

  for (uint8_t i=0; i<3; i++) {
    E_state = ts_digitalRead(E_pin);
    D_state = ts_digitalRead(D_pin);
    if (D_state == HIGH) {
      if (E_state == HIGH) S3_count++;
      else SLEEP_count++;
      }
    else {
      if (E_state == HIGH) WATCH_count++;
      else S0_count++;
      }
    ts_delayUs(BUS_CHECK_DLY);
    }
  if (SLEEP_count >= 2) state = SLEEP_STATE;
  else if (WATCH_count >= 2) state = WATCH_STATE;
  else if (S3_count >= 2) state = S3_STATE;
  else if (S0_count >= 2) state = S0_STATE;
  else state = UNKNOWN_STATE;
  
  return state;
  }

bool TS4231::goToWatch(void) {
  bool watch_success;
  
  if (configured == false)  watch_success = false;
  else {
    switch (checkBus()) {
      case S0_STATE:
        watch_success = false;
        break;
      case SLEEP_STATE:
        ts_digitalWrite(D_pin, HIGH);
        ts_pinMode(D_pin, OUTPUT);
        ts_digitalWrite(E_pin, LOW);
        ts_pinMode(E_pin, OUTPUT);
        ts_digitalWrite(D_pin, LOW);
        ts_pinMode(D_pin, INPUT);
        ts_digitalWrite(E_pin, HIGH);
        ts_pinMode(E_pin, INPUT);
        ts_delayUs(SLEEP_RECOVERY);
        if (checkBus() == WATCH_STATE) watch_success = true;
        else watch_success = false;
        break;
      case WATCH_STATE:
        watch_success = true;
        break;
      case S3_STATE:
        ts_digitalWrite(E_pin, HIGH);
        ts_pinMode(E_pin, OUTPUT);
        ts_digitalWrite(D_pin, HIGH);
        ts_pinMode(D_pin, OUTPUT);
        ts_digitalWrite(E_pin, LOW);
        ts_digitalWrite(D_pin, LOW);
        ts_pinMode(D_pin, INPUT);
        ts_digitalWrite(E_pin, HIGH);
        ts_pinMode(E_pin, INPUT);
        ts_delayUs(SLEEP_RECOVERY);
        if (checkBus() == WATCH_STATE) watch_success = true;
        else watch_success = false;
        break;
      default:
        watch_success = false;
        break;
      }
    }
  return watch_success;
  }
