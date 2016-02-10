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


void loop() {  
  
    //lub
    for (int y = 0; y < 1500; y+=50) {
      for (int x = 0; x < 5; x++) {
        setPWM(x,y);
      }
    }
    for (int y = 0; y < 1500; y+=25) {
      for (int x = 0; x < 5; x++) {
        setPWM(4-x,2048-y);
      }
    }
    
    //dub
    for (int y = 0; y < 4096; y+=64) {
      for (int x = 0; x < 5; x++) {
        setPWM(x,y);
      }
    }
    delay(15);
    for (int y = 0; y < 4096; y+=16) {
      for (int x = 0; x < 5; x++) {
        setPWM(4-x,4096-y);
      }
    }
    
  delay(1000);
  
  

}

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
