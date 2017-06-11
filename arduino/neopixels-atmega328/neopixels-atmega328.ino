#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "Wire.h"

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

const int ACK = 6;
const int NAK = 21;

const int I2C_ADDRESS = 0x26;

const int PIN_LED_1 = 10;
const int PIN_LED_2 = 9;
const int PIN_LED_BUSY = 11;

const int NEOPIXEL_PIN  = 4;

const int CMD_INITIALIZE    = 0x10;  // size
const int CMD_SET_COLOR     = 0x11;  // red, green, blue
const int CMD_FADE_TO_COLOR = 0x12;  // red, green, blue
const int CMD_WIPE_TO_COLOR = 0x13;  // red, green, blue, delay
const int CMD_SET_PIXELS    = 0x14;  // array of red, green, blue


const int ERR_OK                = 0;
const int ERR_INVALID_PARAMETER = 1;
const int ERR_PARAMETER_MISSING = 2;
const int ERR_NOT_INITIALIZED   = 3;
const int ERR_INVALID_COMMAND   = 4;
const int ERR_OUT_OF_MEMORY     = 5;


typedef struct RGB {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

static RGB rgb[240];

//static RGB rgb[240];

//////////////////////////////////////////////////////////////////////////////////////////////////////

class NeopixelStrip {


    public:
        
        NeopixelStrip(int length) : _strip(length, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800) {
            _strip.begin();
        };

        int numPixels() {
            _strip.numPixels();
        }
        
        void setColor(int red, int green, int blue) {
        
            for (int i = 0; i < _strip.numPixels(); i++) {
                _strip.setPixelColor(i, red, green, blue);
            }
        
            _strip.show();
        }    

        void setPixelColor(int i, int red, int green, int blue) {            
          _strip.setPixelColor(i, rgb->red, rgb->green, rgb->blue);
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

class LedLamp {

    public:
        
        LedLamp(int pin = -1) {

            if (pin >= 0)
                setPin(pin);
        }

        void setPin(int pin) {
            _pin = pin;
            _state = LOW;
            
            pinMode(pin, OUTPUT);
            digitalWrite(pin, _state);
        }

        void setState(int state) {
            _state = state ? HIGH : LOW;
            digitalWrite(_pin, _state);
        }
        
        void toggleState() {
            setState(_state == LOW ? HIGH : LOW);
        }

        void blink(int blinks = 1, int wait = 200) {
            for (int i = 0; i < blinks; i++) {
                toggleState();
                delay(wait);
            }
        }

    private:
        int _state;
        int _pin;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

static void *_app = NULL;

class App {

    
    public:

        App() {
            _app = (void *)this;
            _strip = NULL;
            _status = ACK;

            _error.setPin(PIN_LED_1);
            _heartbeat.setPin(PIN_LED_2);
            _busy.setPin(PIN_LED_BUSY);
        }
        
        void setup() {
            Wire.begin(I2C_ADDRESS);
            Wire.onReceive(App::receive);
            Wire.onRequest(App::request);            

            _error.blink(2, 250);

             _strip = new NeopixelStrip(10);
             _strip->setColor(64, 0, 0);            
        }
    
        void loop() {
            _heartbeat.toggleState();
            delay(100);
        };

        static void receive(int bytes) {
            ((App *)_app)->onReceive(bytes);
        };
        
        static void request() {
            ((App *)_app)->onRequest();
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
                        return ERR_PARAMETER_MISSING;
        
                    if (length < 0 || length > 240)
                        return ERR_INVALID_PARAMETER;
                        
                    delete _strip;
                    _strip = new NeopixelStrip(length);

                    break;
                }

                case CMD_SET_PIXELS: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red, green, blue;
                    int numPixels = _strip->numPixels();
                        
                    for (int i = 0; i < numPixels; i++) {
                        if (!readRGB(red, green, blue)) {
                            return ERR_PARAMETER_MISSING;
                        }

                        _strip->setPixelColor(i, red, green, blue);
                    }

                    _strip->show();
                    break;
                };

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

                default: {
                    return ERR_INVALID_COMMAND;
                }
            };

            return ERR_OK;
    
        }
        


        void onReceive(int bytes) {
        
            if (Wire.available()) {
                int command = ERR_INVALID_COMMAND, error = ERR_OK;
                
                if (readByte(command)) {
                    _status = NAK;

                    _busy.setState(HIGH);
                    error = onCommand(command);
                    _busy.setState(LOW);

                    _status = ACK;
                
                }

                if (error > 0) {
                    Wire.flush();
                    _error.blink(error);
                }
        
            }
        
            
        
        }

        void onRequest() {
            Wire.write(_status);
        }



    private:

        LedLamp _error, _heartbeat, _busy;
        NeopixelStrip *_strip;
        uint8_t _status;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////


App app;

void setup()
{
    app.setup();
}


void loop() 
{
    app.loop();
}












