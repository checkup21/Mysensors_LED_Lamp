#include "LedUtil.h"

namespace
{
  // Create a 24 bit color value from R,G,B
  uint32_t
  Color(byte r, byte g, byte b)
  {
    uint32_t c;
    c = r;
    c <<= 8;
    c |= g;
    c <<= 8;
    c |= b;
    return c;
  }

  //Input a value 0 to 255 to get a color value.
  //The colours are a transition r - g -b - back to r
  uint32_t
  Wheel(byte WheelPos)
  {
    if (WheelPos < 85) {
      return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
      WheelPos -= 85;
      return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
      WheelPos -= 170;
      return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
  }
}

LedUtil::LedUtil(int num_leds, int ledDataPin, int ledClockPin)
:ledStrip(num_leds, ledDataPin, ledClockPin)
, num_leds(num_leds)
, cvalues{0, 0, 0}
{
}

void 
LedUtil::init()
{
  ledStrip.begin();
  // Update LED contents, to start they are all 'off'
  ledStrip.show();
}

void 
LedUtil::setPixel(int pixel, byte red, byte green, byte blue) {
  ledStrip.setPixelColor(pixel, red, green, blue);
}

void 
LedUtil::setAll()
{
    for (int i = 0; i < num_leds; i++) {
      setPixel(i, cvalues[0], cvalues[1], cvalues[2]);
    }

    ledStrip.show();   // write all the pixels out
}

void 
LedUtil::setAllVal(byte red, byte green, byte blue) {
  cvalues[0] = red;
  cvalues[1] = green;
  cvalues[2] = blue;
  setAll();
}

void 
LedUtil::waterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j < 3; j++) {  //do 3 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < num_leds; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      ledStrip.show();
     
      delay(SpeedDelay);
     
      for (int i=0; i < num_leds; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void
LedUtil::colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i = 0; i < ledStrip.numPixels(); i++) {
    ledStrip.setPixelColor(i, c);
    ledStrip.show();
    delay(wait);
  }
}

void
LedUtil::rainbow(uint8_t wait) {
  for (rainbowJ = 0; rainbowJ < 256; rainbowJ++) {   // 3 cycles of all 256 colors in the wheel
    for (int i = 0; i < ledStrip.numPixels(); i++) {
      ledStrip.setPixelColor(i, Wheel( (i + rainbowJ) % 255));
    }
    ledStrip.show();   // write all the pixels out
    if (rainbowJ >= 256)
      rainbowJ = 0;
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void
LedUtil::rainbowCycle(uint8_t wait)
{  
  //loop from the caller...
  //for (rainbowJ = 0; rainbowJ < 256; rainbowJ++) {   // 5 cycles of all 25 colors in the wheel
  for (int i = 0; i < ledStrip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      ledStrip.setPixelColor(i, Wheel( ((i * 256 / ledStrip.numPixels()) + rainbowJ) % 256) );
    }
    ledStrip.show();   // write all the pixels out
    if (rainbowJ++ >= 256)
      rainbowJ = 0;
    delay(wait); 
  
  // }
}

void 
LedUtil::Strobe(byte red, byte green, byte blue, int StrobeCount, 
       int FlashDelay)
{
  for(int j = 0; j < StrobeCount; j++) 
  {
    setAllVal(red,green,blue);
    delay(FlashDelay);
    setAllVal(0,0,0);  
    delay(FlashDelay);
  }   
}

void 
LedUtil::CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, 
            int ReturnDelay){

  for(int i = 0; i < num_leds-EyeSize-2; i++) 
  {
    setAllVal(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    ledStrip.show(); 

    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = num_leds-EyeSize-2; i > 0; i--) {
    setAllVal(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    ledStrip.show(); 
    delay(SpeedDelay);
  }  
  delay(ReturnDelay);
}

void 
LedUtil::Fire(int Cooling, int Sparking, int SpeedDelay) 
{
  //static byte heat[num_leds];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < num_leds; i++) {
    cooldown = random(0, ((Cooling * 10) / num_leds) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= num_leds - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < num_leds; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  ledStrip.show();
  delay(SpeedDelay);
}

void 
LedUtil::setPixelHeatColor(int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

