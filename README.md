# TS4231 Library Usage

**** First, install the Arduino library. **********************************

The TS4231 Arduino library is able to be downloaded at the following link:

https://github.com/TriadSemi/TS4231

Installation instructions:

1) Navigate to the above link
2) Click on the Clone or download button
3) Select Download ZIP and save to any location you choose
4) Open the Arduino IDE
5) Select menu option Sketch > Include Library > Add .ZIP Library
6) Navigate to the ZIP file you downloaded from GitHub, then select it and click Open
7) That is all.  The library is now installed.

**** Next, set up your hardware and run the example application ************

**** IMPORTANT NOTES BEFORE BEGINNING
**** 1) The TS4231 is a 3.3V device and must only be used with a 3.3V Arduino board.  Failure to do so will damage the TS4231.
**** 2) DO NOT add pull-up or pull-down resistors to the D and E signals.
**** 3) DO NOT configure the Arduino INPUT ports that connect to the D and E signals with a pull-up or pull-down function.  They must be floating.
**** 4) If using a microcontroller than can toggle output pin states faster than approximately 100ns, the TS4231 datasheet must be consulted to verify TS4231 timing parameters are not being violated to assure proper TS4231 operation.

1) First, install the TS4231 Arduino library by following the instructions here:

http://help.triadsemi.com/ts4231-light-to-digital-ic-with-data-output/how-to-install-your-ts4231-arduino-library

2) After the library is installed, modify the example application for your Adruino board.

Open the TS4231 example application in the Arduino IDE by selecting File > Examples > TS4231-master > ts4231_config_example.

Determine which 2 pins you will use on your Arduino board to connect to the TS4231 E and D signal pins.  The pins you choose must be bidirectional (supporting INPUT and OUTPUT pin modes).

Modify the following lines of code in the example application by replacing your_E_pin and your_D_pin with the pin numbers you have chosen:

#define sensor1_E_pin  your_E_pin   //User must replace your_E_pin with their pin number (compile error will occur if no number defined)

#define sensor1_D_pin  your_D_pin   //User must replace your_D_pin with their pin number (compile error will occur if no number defined)

** Note:  Your particular Arduino board may require that you make additional code modifications to the example application, so be sure to familiarize yourself with your hardware and its requirements.

3) After the example application has been modified to add your pin numbers, set up your hardware.  You will need the following:

    - Your Arduino board (remember, it MUST be a 3.3V board)
    - TS4231 board with documentation for pin assignments
    - Lighthouse base station
    - 3.3V power supply or 3.3V output on your Arduino board

With all power turned off, hook up the TS4231 board's power and ground pins to a 3.3V power supply (refer to your TS4231 documentation for pin assignments).  It is highly recommended that you use a 3.3V power supply output and GND pin on your Arduino board.  It will make things much easier.  Then, connect the Arduino E and D pins that you assigned in step 2 to the TS4231 board's E and D pins.  That is it, just 4 connections.

** Note:  If you are NOT using 3.3V / GND pins from your Arduino board, you will need to connect a GND jumper wire between the GND of your 3.3V power supply and a GND connection on the Arduino board to keep your system at the same ground reference.

Position the component side of the TS4231 board within line-of-sight of your lighthouse base station.  The IR detector on the TS4231 board must not be occluded and must be able to "see" the base station laser sweeps.  Distance between the TS4231 board and base station can be up to 5 meters.

Your hardware is now set up, it is time to run the example application.

4) Upload and run the application

The TS4231 has a volatile configuration so it must be configured after each power-up cycle.  Additionally, the device must detect light from a base station after power up and before configuration.  After light has been detected (using the library's waitForLight() function) it is not necessary to detect light again if a re-configuration becomes necessary.

Power up your lighthouse base station and wait for it to indicate that it is operating.  Since the TS4231 needs to detect light from the base station before it can be configured, it is important that the base station is operational before launching the example application.

If you are NOT using your Arduino board to power the TS4231 board, turn on your 3.3V power supply now.  Then, power up your Arduino board and click the Upload button in the Arduino IDE to upload the example application.

The setup() function in the example application halts execution until the serial port is connected, so even though you have uploaded the application, it is not communicating with the TS4231 until you open the serial monitor.

In the Arduino IDE, open the serial monitor under Tools > Serial Monitor.  As soon as the serial port connects, the setup() function will resume execution and you will see output on the serial monitor indicating that the TS4231 has been successfully configured!
