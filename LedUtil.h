
#ifndef ledutil_h
#define ledutil_h

#include <Adafruit_WS2801.h>

class LedUtil
{
public:
    LedUtil(int num_leds, int ledDataPin, int ledClockPin);

    void init();

    void rainbow(uint8_t wait);
    void rainbowCycle(uint8_t wait);
    void setPixel(int pixel, byte red, byte green, byte blue);
    void setAll();
    void setAllVal(byte red, byte green, byte blue);
    void waterChase(byte red, byte green, byte blue, int SpeedDelay);
    void colorWipe(uint32_t c, uint8_t wait);
    void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay);
    void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay); 
    void Fire(int Cooling, int Sparking, int SpeedDelay);
    void setPixelHeatColor(int Pixel, byte temperature);  

    long cvalues[3];

private:
    Adafruit_WS2801 ledStrip;
    int num_leds;   
    byte heat[50];
    int rainbowJ;
};
#endif

