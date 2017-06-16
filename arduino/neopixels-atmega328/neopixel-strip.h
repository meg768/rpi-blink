#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)



#include "memory.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

class NeopixelStrip {

    private:
        typedef struct RGB {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };

    public:

        NeopixelStrip(int pin, int length) : _strip(length, pin, NEO_BRG + NEO_KHZ800) {
            _strip.begin();
        };

        int numPixels() {
            return _strip.numPixels();
        }

        void setColor(int red, int green, int blue) {

            for (int i = 0; i < _strip.numPixels(); i++) {
                _strip.setPixelColor(i, red, green, blue);
            }

            _strip.show();
        }

        void setPixelColor(int i, int red, int green, int blue) {
            _strip.setPixelColor(i, red, green, blue);
        }

        void show() {
            _strip.show();
        }

        void wipeToColor(int red, int green, int blue, int wait) {

            for (int i = 0; i < _strip.numPixels(); i++) {
                _strip.setPixelColor(i, red, green, blue);
                _strip.show();
                delay(wait);
            }
        }

        void fadeToColor(int red, int green, int blue, int numSteps) {

            int numPixels = _strip.numPixels();

            Memory memory;
            RGB *rgb = memory.alloc(numPixels * sizeof(RGB));

            if (rgb == NULL)
                return setColor(red, green, blue);

            for (int i = 0; i < numPixels; i++) {
                uint32_t color = _strip.getPixelColor(i);
                rgb[i].red   = (int)(uint8_t)(color >> 16);
                rgb[i].green = (int)(uint8_t)(color >> 8);
                rgb[i].blue  = (int)(uint8_t)(color);
            }

            for (int step = 0; step < numSteps; step++) {

                for (int i = 0; i < numPixels; i++) {
                    uint8_t pixelRed   = rgb[i].red   + (step * (red   - rgb[i].red))   / numSteps;
                    uint8_t pixelGreen = rgb[i].green + (step * (green - rgb[i].green)) / numSteps;
                    uint8_t pixelBlue  = rgb[i].blue  + (step * (blue  - rgb[i].blue))  / numSteps;

                    _strip.setPixelColor(i, pixelRed, pixelGreen, pixelBlue);
                }

                _strip.show();

            }

            for (int i = 0; i < numPixels; i++)
                _strip.setPixelColor(i, red, green, blue);


            _strip.show();

        }


    private:
        Adafruit_NeoPixel _strip;
};
