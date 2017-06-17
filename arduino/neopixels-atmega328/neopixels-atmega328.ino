
#ifdef __AVR_ATtiny85__
#include <TinyWireS.h>
#else
#include <Wire.h>
#endif

#include "neopixel-strip.h"
#include "blinker.h"


const uint8_t ACK = 6;
const uint8_t NAK = 21;

const int CMD_INITIALIZE    = 0x10;  // size
const int CMD_SET_COLOR     = 0x11;  // red, green, blue
const int CMD_FADE_TO_COLOR = 0x12;  // red, green, blue
const int CMD_WIPE_TO_COLOR = 0x13;  // red, green, blue, delay
const int CMD_SET_PIXEL     = 0x14;  //
const int CMD_REFRESH       = 0x15;  //

const int ERR_OK                = 0;
const int ERR_INVALID_PARAMETER = 1;
const int ERR_PARAMETER_MISSING = 2;
const int ERR_NOT_INITIALIZED   = 3;
const int ERR_INVALID_COMMAND   = 4;

static void *_app = NULL;



#ifdef __AVR_ATtiny85__
#include "attiny85.h"
App app(0x26, 1, 4);
#else
#include "atmega328.h"
App app(0x26, 4, 20);

#endif



void setup()
{
    app.setup();
}


void loop()
{
    app.loop();
}
