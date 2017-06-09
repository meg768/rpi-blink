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

#define I2C_ADDRESS       0x26

#define PIN_LED_1         10
#define PIN_LED_2         9

//#define NEOPIXEL_LENGTH   8
#define NEOPIXEL_PIN      4

#define CMD_SET_COLOR     0x10  // red, green, blue
#define CMD_COLOR_WIPE    0x11  // red, green, blue, delay
#define CMD_SET_LENGTH    0x12  // size
#define CMD_FADE_IN       0x13
#define CMD_DEMO          0x14

Adafruit_NeoPixel *strip = NULL;

void setup()
{
    pinMode(PIN_LED_1, OUTPUT);
    pinMode(PIN_LED_2, OUTPUT);

    blink(PIN_LED_1, 3);
    blink(PIN_LED_2, 3);


    Wire.begin(I2C_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

void loop() 
{
    static int state = 0;
    digitalWrite(PIN_LED_2, state == 0 ? LOW : HIGH);
    state = !state;

    delay(100);
}

void sendData()
{
}


void receiveData(int bytes)
{
    
    if (Wire.available()) {
        byte command = Wire.read();
        byte reply = 0;

        switch (command) {
        /*
            case CMD_SET_COLOR: {
                reply = cmdSetColor();
                break;
            };
    
            case CMD_COLOR_WIPE: {
                reply = cmdColorWipe();
                break;
            }
          */
            case CMD_SET_LENGTH: {
                reply = cmdSetLength();
                break;
            }
            /*
            case CMD_FADE_IN: {
                reply = cmdFadeIn();
                break;
            }
            */
            case CMD_DEMO: {
                reply = cmdDemo();
                break;
            }
        };

        if (reply > 0) {
            flush();
            blink(PIN_LED_1, reply);
        }
    }

    

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


int flush()
{
    while (Wire.available()) {
        Wire.read();
        delay(100);
    }

    return 0;
}


int readByte() {

    long timeout = millis() + 200;
    
    for (;;) {
        if (Wire.available())
            return (int)Wire.read();

        if (millis() > timeout)
            return -1;
    }

}




byte cmdSetLength() {

    int length = 0;

    if ((length = readByte()) == -1)
        return 1;

    delete strip;
    strip = new Adafruit_NeoPixel(length, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


    strip->begin();

    
    return 0;

}


byte cmdFadeIn() {

    int red, green, blue, numSteps;

    if (strip == NULL)
        return 10;
        
    if ((red = readByte()) == -1)
        return 1;

    if ((green = readByte()) == -1)
        return 2;

    if ((blue = readByte()) == -1)
        return 3;

    if ((numSteps = readByte()) == -1)
        return 4;

    uint32_t color = strip->Color(red, green, blue);

    int numPixels = strip->numPixels();

    static uint32_t rgb[255];
        
    // Save pixel colors
    for (int i = 0; i < numPixels; i++) {
        rgb[i] = strip->getPixelColor(i);
    }

    for (int step = 0; step < numSteps; step++) {
    
        for (int i = 0; i < numPixels; i++) {
            int r = (int)(uint8_t)(rgb[i] >> 16);
            int g = (int)(uint8_t)(rgb[i] >> 8);
            int b = (int)(uint8_t)(rgb[i]);
            
            uint8_t pixelRed   = r + (step * (red   - r)) / numSteps; 
            uint8_t pixelGreen = g + (step * (green - g)) / numSteps;
            uint8_t pixelBlue  = b + (step * (blue  - b)) / numSteps;

            strip->setPixelColor(i, pixelRed, pixelGreen, pixelBlue);
        }

        strip->show();
        delay(1);
    
    }

    for (int i = 0; i < numPixels; i++)
        strip->setPixelColor(i, strip->Color(red, green, blue));
    
    strip->show();

    // Clean up
    //free(rgb);

    return 0;
}


byte cmdSetColor() {
    int red, green, blue;

    if (strip == NULL)
        return 10;

        
    if ((red = readByte()) == -1)
        return 1;

    if ((green = readByte()) == -1)
        return 2;

    if ((blue = readByte()) == -1)
        return 3;


    for (uint16_t i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, red, green, blue);
    }

    strip->show();

    return 0;
}

/*
byte cmdColorWipe() {
    int red, green, blue, wait;
    
    if (strip == NULL)
        return 10;


    if ((red = readByte()) == -1)
        return 1;

    if ((green = readByte()) == -1)
        return 2;

    if ((blue = readByte()) == -1)
        return 3;

    if ((wait = readByte()) == -1)
        return 4;

    for (uint16_t i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, red, green, blue);
        strip->show();
        delay(wait);
    }

    return 0;
}
*/
byte cmdDemo() {


    if (strip == NULL)
        return 10;

    uint32_t color = strip->Color(128, 0, 0);

    for (uint16_t i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, 0, 0, 0);
    }
    
    strip->show();
    
    for(uint16_t i=0; i<strip->numPixels()+4; i++) {
        strip->setPixelColor(i  , color); // Draw new pixel
        strip->setPixelColor(i-4, 0); // Erase pixel a few steps back
        strip->show();
        delay(25);
    }

    for (uint16_t i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, 0, 0, 0);
    }

    strip->show();
    return 0;
}



