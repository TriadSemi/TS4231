/*******************************************************************
    Copyright (C) 2017 Triad Semiconductor
    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.
    4. Use of this source code or binary forms in commercial products shall require explicit
       written consent of Triad Semiconductor
    5. Silicon ASIC implementation of algorithims in this source is strictly prohibited, FPGA versions
       shall be permitted provided the source is open and attribution is made to this original work.
    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    ts4231_config_example.ino - Example application for configuring the Triad
              Semiconductor TS4231 Light to Digital converter.
    Created by: John Seibel
*******************************************************************/
#include <ts4231.h>

#define light_timeout   500  //500ms is a placeholder as this number will be system dependent
#define device1_E_pin   your_E_pin   //User must replace your_E_pin with their pin number (compile error will occur if no number defined)
#define device1_D_pin   your_D_pin   //User must replace your_D_pin with their pin number (compile error will occur if no number defined)

uint8_t config_result;

TS4231  device1(device1_E_pin, device1_D_pin);  //instantiate the class as device1 and assign pins

void setup() {
  Serial.begin(9600);
  while (!Serial);  //wait for serial port to connect

  Serial.println("Serial Port Connected");
  Serial.println();
  Serial.println();

//The application must verify that each TS4231 has detected light prior to
//configuring the device.  The waitForLight() function is executed after power-up
//on an un-configured device.  Once the device has detected light, waitForLight()
//does not need to be executed again unless the device loses power.
  if (device1.waitForLight(light_timeout)) {
    
    //Execute this code when light is detected
    Serial.println("Light DETECTED");
    
    config_result = device1.configDevice();
  
    //user can determine how to handle each return value for the configuration function
    switch (config_result) {
      case CONFIG_PASS:
        Serial.println("Configuration SUCCESS");
        break;
      case BUS_FAIL:  //unable to resolve state of TS4231 (3 samples of the bus signals resulted in 3 different states)
        Serial.println("Configuration Unsuccessful - BUS_FAIL");
        break;
      case VERIFY_FAIL:  //configuration read value did not match configuration write value, run configuration again
        Serial.println("Configuration Unsuccessful - VERIFY_FAIL");
        break;
      case WATCH_FAIL:  //verify succeeded but entry into WATCH mode failed, run configuration again
        Serial.println("Configuration Unsuccessful - WATCH_FAIL");
        break;
      default:  //value returned was unknown
        Serial.println("Program Execution ERROR");
        break;
      }
    }
  else {
    //insert code here for no light detection
    Serial.println("Light TIMEOUT");
    }
  Serial.println("");
  }
  
void loop() {
  //insert your main code here
  }
