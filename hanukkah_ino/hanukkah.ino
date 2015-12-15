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

for (int x = 5; x < 10; x++) {
  setPWM(x,4095);
}

setPWM(2,4095); setPWM(12,4095);

}

void loop()
{  
  
  
  
  for (int x = 0; x < 4096; x+=10) {
    setPWM(0,x); setPWM(4,x); setPWM(10,x); setPWM(14,x); //transition 1st and 5th to white    
  }
  delay(100);
  
  for (int x = 0; x < 3500; x+=10) {
    setPWM(1,x); setPWM(3,x); setPWM(11,x); setPWM(13,x); //start transition 2nd and 4th to white    
  }
  
  for (int x = 0; x < 595; x+=10) {
    setPWM(1,x+3500); setPWM(3,x+3500); setPWM(11,x+3500); setPWM(13,x+3500); //finish transition 2nd and 4th to white
    setPWM(0,4095-x); setPWM(4,4095-x); setPWM(10,4095-x); setPWM(14,4095-x); //start transition 1st and 5th back to blue
  }
   
  for (int x = 0; x < 3500; x+=8) {
    setPWM(0,3500-x); setPWM(4,3500-x); setPWM(10,3500-x); setPWM(14,3500-x); //finish transition 1st and 5th back to blue
  }
  delay(100);
  
  for (int x = 0; x < 4096; x+=5) {
    setPWM(1,4095-x); setPWM(3,4095-x); setPWM(11,4095-x); setPWM(13,4095-x); //transition 2nd and 4th to back to blue   
  }
  delay(2000);
  
  
  

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
