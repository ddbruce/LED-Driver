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
// begin configure seven seg driver
  pinMode(A1, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A2, OUTPUT);
// end configure seven seg driver

// configure pushbutton
  pinMode(2, INPUT);
attachInterrupt(0,checkMode,FALLING); // setup interrupt

// configure serial and i2c communication
Serial.begin(9600);
Wire.begin();

// configure PWM chip
setConfiguration();
}

void loop()
{  
//checkMode();
bP = false; // clear ISR flag
if ((millis() - modeSetTime) > 2000)
{ 
setSevenSeg(10); // disable seven seg after 2000 milliseconds
sevSegOff = true;
}

// defines what each mode does
switch (mode)
{
  case 1:
// test every channel by fading up, then down
  for (int i = 0; i < 10; i += 1)
  {
  for (int j = 0; j < 4095; j += 10*readPot())//scale on pot value
  {
    setPWM(i, j);
    if (bP) break;
  }
  for (int j = 4095; j>0; j -= 10*readPot())
  {
    setPWM(i, j);
    if (bP) break;
  }
  if (bP) break;
  delay(100);
}
break;
case 2:
  //knight rider, no tail, red background, white chase
for(int i = 0; i<5; ++i) setPWM(i,4095);
  for(int i = 4; i<10; ++i) setPWM(i,0);
    for(int k = 0; k<5; ++k){
    for (int i = 0; i<4096; i += 20*readPot())//scale on pot value
    {
      setPWM(9-k,i);
      setPWM(4-k,4095-i);
      if (bP) break;
    }
    for (int i = 4095; i>0; i -= 20*readPot())
    {
      setPWM(9-k,i);
      setPWM(4-k,4095-i);
      if (bP) break;
    }
    if (bP) break;
  }
  for(int k = 3; k>0; --k){
    for (int i = 0; i<4096; i += 20*readPot()) 
    {
      setPWM(9-k,i);
      setPWM(4-k,4095-i);
      if (bP) break;
    }
    for (int i = 4095; i>0; i -= 20*readPot())
    {
      setPWM(9-k,i);
      setPWM(4-k,4095-i);
      if (bP) break;
    }
    if (bP) break;
  }
  break;
  case 3: //pings white on red background randomly for ~30 seconds, then reverses colors
  unsigned long startTime;
  int randTime;
  int randChannel;
  int randChannelOther;
  int randChannelLast;
  
    for (int j = 0; j < 4096; j += 20) { //initialize all red on, all white off
        setPWM(0,j);
        setPWM(1,j);
        setPWM(2,j);
        setPWM(3,j);
        setPWM(4,j);
        setPWM(5,4095-j);
        setPWM(6,4095-j);
        setPWM(7,4095-j);
        setPWM(8,4095-j);
        setPWM(9,4095-j);
        if (bP) break;
    }
    
    
  while(1)
  {
    startTime = millis();
    randChannelLast = 10;
    while ((millis() - startTime) < 60000) {
      randChannel = floor(random(5,10));
      while (randChannel == randChannelLast) randChannel = floor(random(5,10));
      randChannelOther = randChannel - 5;
      randTime = floor(random(1200,2000));
      for (int i = 0; i < 4096; i += 75) {
        setPWM(randChannel,i);
        setPWM(randChannelOther,4095-i);
      }
      for (int i = 4065; i > 0; i -= 5000/randTime) {
        setPWM(randChannel,i);
        setPWM(randChannelOther,4095-i);
        if (bP) break;
      }
      randChannelLast = randChannel;
      if (bP) break;
    }

    for (int j = 0; j < 4096; j += 20) {
        setPWM(5,j);
        setPWM(6,j);
        setPWM(7,j);
        setPWM(8,j);
        setPWM(9,j);
        setPWM(0,4095-j);
        setPWM(1,4095-j);
        setPWM(2,4095-j);
        setPWM(3,4095-j);
        setPWM(4,4095-j);
        if (bP) break;
    }

    startTime = millis();
    while ((millis() - startTime) < 60000) {
      randChannel = floor(random(0,5));
      while (randChannel == randChannelLast) randChannel = floor(random(0,5));
      randChannelOther = randChannel + 5;
      randTime = floor(random(1200,2000));
      for (int i = 0; i < 4096; i += 75) {
        setPWM(randChannel,i);
        setPWM(randChannelOther,4095-i);
      }
      delay(50);
      for (int i = 4065; i > 0; i -= 5000/randTime) {
        setPWM(randChannel,i);
        setPWM(randChannelOther,4095-i);
        if (bP) break;
      }
      randChannelLast = randChannel;
      if (bP) break;
    } 
    for (int j = 0; j < 4096; j += 20) {
        setPWM(0,j);
        setPWM(1,j);
        setPWM(2,j);
        setPWM(3,j);
        setPWM(4,j);
        setPWM(5,4095-j);
        setPWM(6,4095-j);
        setPWM(7,4095-j);
        setPWM(8,4095-j);
        setPWM(9,4095-j);
        if (bP) break;
    }
        
    if (bP) break;
  }
  break;
  default:
  break;
}
}

/*
* function to control the seven seg display, display value
*/
void setSevenSeg (int value)
{
  switch(value)
  {
    case 0:
    digitalWrite(A1, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(A2, LOW);
    break;
    case 1:
    digitalWrite(A1, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(A2, LOW);
    break;
    case 2:
    digitalWrite(A1, LOW);
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(A2, LOW);
    break;
    case 3:
    digitalWrite(A1, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(A2, LOW);
    break;
    case 4:
    digitalWrite(A1, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(A2, LOW);
    break;
    case 5:
    digitalWrite(A1, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(A2, LOW);
    break;
    case 6:
    digitalWrite(A1, LOW);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(A2, LOW);
    break;
    case 7:
    digitalWrite(A1, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(A2, LOW);
    break;
    case 8:
    digitalWrite(A1, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(A2, HIGH);
    break;
    case 9:
    digitalWrite(A1, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(A2, HIGH);
    break;
default: // blank screen for numbers not in 0-9
digitalWrite(A1, HIGH);
digitalWrite(6, HIGH);
digitalWrite(7, HIGH);
digitalWrite(A2, HIGH);
}
}

/*
* increments mode and displays value on seven seg, triggered by ISR
*/

void checkMode()
{
  mode++;
modeSetTime = millis(); // set time mode was set so seven seg can be turned off after
                            // duration
if (mode>3) mode = 0;
setSevenSeg(mode); //set seven seg to mode number
sevSegOff = false;
bP = true;
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

/*
* function for returning a scaled value from 0 to 1
*/
float readPot()
{
potValue = analogRead(A0); // save value from potentiometer
return (float)potValue/1023;   
}
