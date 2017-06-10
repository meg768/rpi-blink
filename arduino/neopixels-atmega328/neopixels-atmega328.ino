#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "Wire.h" // wrapper class for I2C slave routines

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

#define ACK               6
#define NAK               21

#define I2C_ADDRESS       0x26

#define PIN_LED_1         10
#define PIN_LED_2         9

#define NEOPIXEL_PIN      4

#define CMD_INITIALIZE    0x10  // size
#define CMD_SET_COLOR     0x11  // red, green, blue
#define CMD_FADE_TO_COLOR 0x12  // red, green, blue
#define CMD_WIPE_TO_COLOR 0x13  // red, green, blue, delay
#define CMD_DEMO          0x14


const int ERR_OK                = 0;
const int ERR_INVALID_X         = 1;
const int ERR_INVALID_PARAMETER = 2;
const int ERR_PARAMETER_MISSING = 3;
const int ERR_NOT_INITIALIZED   = 4;
const int ERR_INVALID_COMMAND   = 5;


//////////////////////////////////////////////////////////////////////////////////////////////////////

class NeopixelStrip {


    public:
        
        NeopixelStrip(int length) : _strip(length, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800) {
        };

        void setColor(int red, int green, int blue) {
        
            for (int i = 0; i < _strip.numPixels(); i++) {
                _strip.setPixelColor(i, red, green, blue);
            }
        
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
        
            struct RGB {
                uint8_t red;
                uint8_t green;
                uint8_t blue;
            }
        
            static rgb[240];
            
            int numPixels = _strip.numPixels();
        
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


//////////////////////////////////////////////////////////////////////////////////////////////////////

class App {

    public:

        App() {
            _strip = NULL;
            _status = ACK;
        }
        
        void setup() {
            pinMode(PIN_LED_1, OUTPUT);
            pinMode(PIN_LED_2, OUTPUT);
        
            blink(PIN_LED_1, 3);
            blink(PIN_LED_2, 3);
        
        }
    
        void loop() {
            static int state = 0;
            digitalWrite(PIN_LED_2, state == 0 ? LOW : HIGH);
            state = !state;
            delay(50);
        };

        int available() {
            return Wire.available();
        
        };
        
        int readByte(int &byte) {
        
            long timeout = millis() + 200;
            
            for (;;) {
                if (Wire.available()) {
                    byte = (int)Wire.read();
                    return true;
                }
        
                if (millis() > timeout)
                    return false;
            }
        
        };
        
        int readRGB(int &red, int &green, int &blue) {
        
            return readByte(red) && readByte(green) && readByte(blue);
            
        };

        int onCommand(int command) {
            
            switch (command) {
                case CMD_INITIALIZE: {
                    int length = 0;
        
                    if (!readByte(length))
                        return ERR_INVALID_PARAMETER;
        
                    if (length < 0 || length > 240)
                        return ERR_INVALID_PARAMETER;
                        
                    delete _strip;
                    _strip = new NeopixelStrip(length);

                    break;
                }
            
                case CMD_SET_COLOR: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;
                        
                    int red = 0, green = 0, blue = 0;
    
                    if (!readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;
                    
                    _strip->setColor(red, green, blue);

                    break;
                };
        
                case CMD_WIPE_TO_COLOR: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, delay = 0;
    
                    if (!readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;
                    
                    if (!readByte(delay))
                        return ERR_INVALID_PARAMETER;
    
                    _strip->wipeToColor(red, green, blue, delay);

                    break;
                }
    
                case CMD_FADE_TO_COLOR: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, steps = 0;
    
                    if (!readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;
                    
                    if (!readByte(steps))
                        return ERR_INVALID_PARAMETER;
    
                    _strip->fadeToColor(red, green, blue, steps);

                    break;
                }
            };

            return ERR_OK;
    
        }
        

        
        void onReceive(int bytes) {
        
            if (Wire.available()) {
                int command = 0;
                int reply = 0;
                
                if (readByte(command)) {
                    _status = NAK;
                    reply = onCommand(command);
                    _status = ACK;
                }
                else 
                    reply = ERR_INVALID_COMMAND;

                if (reply > 0)
                    blink(PIN_LED_1, reply);
        
            }
        
            
        
        }

        void onRequest() {
            Wire.write(_status);
        }

        
        void blink(byte led, byte times)
        {
            for (byte i = 0; i < times; i++) {
                digitalWrite(led, HIGH);
                delay(200);
                digitalWrite(led, LOW);
                delay(100);
            }
        }
        


    private:
    
        NeopixelStrip *_strip;
        uint8_t _status;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////


App app;

void setup()
{
    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    
    app.setup();
}

void onReceive(int bytes) {
    app.onReceive(bytes);
}

void onRequest() {
    app.onRequest();
}


void loop() 
{
    app.loop();
}












