/* 16 Channel LED Driver Board for /The Polytechnic/.
*
* Copyright 2014 Ethan Spitz
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Connections:
* # Potentiometer: A0
* # Button: D2
* # Seven-Segment Display Driver
*    # A: A1 //LSB
*    # B: 6
*    # C: 7
*    # D: A2 //MSB
* # 
*
*/

#include <Wire.h>

volatile int mode = 0; // current mode
int button = 2; // pin of button
volatile bool bP = false; // button pressed flag to exit current modes

int potValue = 0; // value of pot
volatile bool sevSegOff = true;

// values used for mode set/buttton debouncing
volatile long modeSetTime = 0;

// definition of registers on the pwm chip
#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define PWM_ADDR 0x40 //address of the PWM chip


void setup()
{

// configure serial and i2c communication
Serial.begin(9600);
Wire.begin();

// configure PWM chip
setConfiguration();

  
}

void loop()
{  
  
  setPWM(0,4095); //red
  setPWM(2,4095); //red
  setPWM(4,4095); //red
  
  
  setPWM(1,4095); setPWM(5,4095); setPWM(6,4095); setPWM(10,4095); setPWM(11,4095); //white
  setPWM(3,4095); setPWM(8,4095); setPWM(9,4095); setPWM(13,4095); setPWM(14,4095); //white
  
  
  
}

/*
* increments mode and displays value on seven seg, triggered by ISR
*/


/*
* setPWM brightness on channel given over I2C
*/
void setPWM(int channel, uint16_t brightness)
{
  channel = (channel >= 5) ? channel += 1 : channel;
  Wire.beginTransmission(PWM_ADDR);
  Wire.write(LED0_ON_L+4*channel);
  Wire.write(0x00); //turn the LED on at 0
  Wire.write(0x00); //turn the LED on at 0

//turn the LED off when it hits this value (out of 4095)
Wire.write(brightness); //first four LSB
Wire.write(brightness>>8); //last four MSB
Wire.endTransmission();
}

/*
* Configure the PWM chip for easy suage and external MOSFET drive
*/
void setConfiguration()
{
  Wire.beginTransmission(PWM_ADDR);
Wire.write(0x00); //enter Mode 1 Register
Wire.write(0xa1); //enable ocsillator and auto-increment register and restart
Wire.endTransmission();
delayMicroseconds(500);//500ms delay required after reset
Wire.beginTransmission(PWM_ADDR);
Wire.write(0x01); //enter Mode 2 Register
Wire.write(0x04); //set drive mode for external MOSFETS 
Wire.endTransmission();
}

/*
* function for returning a scaled value from 0 to 1
*/
float readPot()
{
potValue = analogRead(A0); // save value from potentiometer
return (float)potValue/1023;   
}
