/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * Sketch to build an LED lamp with 
 ** WS2801 (64 LEDs)
 ** LM35 - temperature
 ** Buzzer 
 **  
 * Support for 
 ** log messages
 * 
 * Sketch consists of 3 files
 ** (this file)
 ** LedUtil.h
 ** LedUtil.cpp
 * 
 * DC supply: 5V
 * 
 * Connections:
 * 
 * LED_clockpin to D4 (green)
 * LED_datapin to D5  (yellow)
 * Buzzer to D7 
 * LM35 to A0
 * 
 * 
 * Contribution by Marco Strack (checkup)
 * Based on https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
 * License: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
 *
 */

 
/*
  All mysensors configuration options have to be set befgore including "MySensors.h"
*/

// MYSENSORS CONFIG START

// Enable debug prints to serial monitor (this sets serial to 115k baud)
//#define MY_DEBUG

#define MY_RADIO_NRF24

//if you want to set (or not set) this and the code fails at runtime,
//flash "mysensors_eeprom_clear"
#define MY_NODE_ID 11

// MYSENSORS CONFIG END

#include <SPI.h>
#include <MySensors.h>
#include "LEDutil.h"

#define ledClockPin 4    // Green wire on Adafruit Pixels
#define ledDataPin  5     // Yellow wire on Adafruit Pixels

#define buzzerPin 7
#define lm35Pin A0

//sensors
#define SENS_ID_TEMP 0
#define SENS_ID_LOG 1

//actors
#define ACT_BUZZER 2
#define ACT_RGB_LIGHT 3

#define NUM_LEDS 64

LedUtil ledUtil(NUM_LEDS, ledDataPin, ledClockPin);

double lastTemp;

// Initialize messages
MyMessage msgTemp(SENS_ID_TEMP, V_TEMP);

//debug sensor, used to send log messages. 
MyMessage msgLog(SENS_ID_LOG, V_TEXT);

void
setup()
{
  //for the lm35
  analogReference(INTERNAL);

  ledUtil.init();
  pinMode(buzzerPin, OUTPUT);
}

void
presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("led.strip", "0.3");

  present(SENS_ID_TEMP, S_TEMP);
  present(SENS_ID_LOG, S_INFO);
  
  present(ACT_BUZZER,  S_BINARY);
  present(ACT_RGB_LIGHT, S_RGB_LIGHT);
}

enum LEDMODE {
  NONE,
  OFF,
  ALL,
  RAINBOW,
  FIRE,
  STROBE,
  CYLON,
  WATER,
  BLINK
} ledMode;

LEDMODE oldLedMode = NONE;

void 
handleLEDs()
{
  switch(ledMode)
  {
    case OFF:
      ledUtil.setAllVal(0,0,0);
      break;
    case ALL:
      ledUtil.setAll(); //uses cvalue
      break;
    case RAINBOW:
      ledUtil.rainbowCycle(40);
      break;
    case FIRE:
      ledUtil.Fire(55,120,55);
      break;
    case STROBE:
      ledUtil.Strobe(0xff, 0xff, 0xff, 30, 50);
      break;
    case CYLON:
      ledUtil.CylonBounce(0xff, 0, 0, 4, 10, 50);
      ledUtil.setAllVal(0,0,0);
      break;
    case WATER:
      ledUtil.waterChase(0,0,0xff,100);
      break;
    case BLINK:
      ledUtil.Strobe(0xff, 0xf7, 0x0a, 2, 125);
      break;
    default:
      ledUtil.setAllVal(0,0,0);
      break;
  }

  //let these run
  if(ledMode != FIRE 
    && ledMode != WATER
    && ledMode != RAINBOW)
  {
    ledMode = NONE;
  }
}

void
receiveRGB(const MyMessage &message)
{
   String hexstring = message.getString();
    if (hexstring.startsWith("X"))    
    {
      hexstring.remove(0,1);
      if (hexstring.startsWith("OFF"))
      {
         ledMode = OFF;
         //hexstring.remove(0, 3);    
      } 
      else if (hexstring.startsWith("RAINBOW"))
      {
         ledMode = RAINBOW;
      }
      else if (hexstring.startsWith("FIRE"))
      {
         ledMode = FIRE;
      }
      else if (hexstring.startsWith("STROBE"))
      {
         ledMode = STROBE;
      }
      else if (hexstring.startsWith("CYLON"))
      {
         ledMode = CYLON;
      }
      else if (hexstring.startsWith("WATER"))
      {
         ledMode = WATER;
      }
      else if (hexstring.startsWith("BLINK"))
      {
         ledMode = BLINK;
      }
    }
    else
    {
       ledMode = ALL;
       long number = (long) strtol(&hexstring[0], NULL, 16);
       ledUtil.cvalues[0] = number >> 16;
       ledUtil.cvalues[1] = number >> 8 & 0xFF;
       ledUtil.cvalues[2] = number & 0xFF; 
       ledUtil.setAll();           
    }    
}

void
receive(const MyMessage &message)
{
  if (message.type == V_RGB) 
  {
    receiveRGB(message);   
  }
  else if (message.type == V_STATUS)
  {
    tone(buzzerPin, 1000); // Send 1KHz sound signal... 
    delay(100); // Send 1KHz sound signal... 
    noTone(buzzerPin);     // Stop sound...
    delay(50); // Send 1KHz sound signal... 
    tone(buzzerPin, 1000); // Send 1KHz sound signal... 
    delay(100); // Send 1KHz sound signal... 
    noTone(buzzerPin);     // Stop sound...
  }  
}

float
readLM35()
{ 
  //throw away read
  analogRead(lm35Pin);
  
  wait(60);
  
  float val = analogRead(lm35Pin);
  return (1.1 * val * 10.0) / 1024;
}

long COUNTER = 0;

void
loop()
{
  if(oldLedMode != ledMode)
   handleLEDs();

  //do this in loop
  if(ledMode == FIRE 
  || ledMode == WATER
  || ledMode == RAINBOW)
    return;

  if ( COUNTER > 600 )
  {
    float temp = readLM35();
    
    if( temp != lastTemp ) 
      send(msgTemp.set(static_cast<float>(temp), 2));    
       
    lastTemp = temp;
    COUNTER = 0;
  }

  //use wait(), not delay()
  wait(1000);
  COUNTER += 1;
}

