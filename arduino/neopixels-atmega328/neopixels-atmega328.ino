#include <Wire.h>


#include "neopixel-strip.h"
#include "blinker.h"


const int ACK = 6;
const int NAK = 21;

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
const int ERR_OUT_OF_MEMORY     = 5;

static void *_app = NULL;

class IO {

    public:
        virtual void begin(int address) = 0;
        virtual int available() = 0;
        virtual int read() = 0;
        virtual void write(uint8_t byte) = 0;

        
        int waitForAvailableBytes(int bytes)
        {
            long timeout = millis() + 300;

            for (;;) {
                if (available() >= bytes) {
                    return true;
                }

                delay(100);

                if (millis() > timeout)
                    return false;
            }
        }


        int readByte(int &data) {

            if (!waitForAvailableBytes(1))
                return false;

            data = read();

            return true;
        };

        int readRGB(int &red, int &green, int &blue) {

            if (!waitForAvailableBytes(3))
                return false;

            red = read();
            blue = read();
            green = read();

            return true;
        };
};


class WireIO : public IO {

    public:
        virtual void begin(int address) {
            Wire.begin(address);
        };

        virtual int available() {
            return Wire.available();
        };

        virtual int read() {
            return Wire.read();
        };

        virtual void write(uint8_t byte) {
            Wire.write(byte);
        };

};


class App {


    public:

        App(int address, int neopixelPin, int stripLength) {
            _app = (void *)this;
            
            _strip = NULL;
            _status = ACK;
            _address = address;
            _stripLength = stripLength;
            _neopixelPin = neopixelPin;
            
            _heartbeat.setPin(13);
            _busy.setPin(12);
            _error.setPin(11);
            _extra1.setPin(10);
            _extra2.setPin(9);
        }

        void setup() {
            Wire.begin(_address);
            Wire.onReceive(App::receive);
            Wire.onRequest(App::request);

            _error.blink(2, 250);

             _strip = new NeopixelStrip(_stripLength, _neopixelPin);
             _strip->setColor(0, 0, 255);
        }

        void loop() {
            static uint32_t counter = 0;

            counter++;

            if ((counter % 1000) == 0) {
                _heartbeat.toggleState();
            }


        };

        static void receive(int bytes) {
            ((App *)_app)->onReceive(bytes);
        };

        static void request() {
            ((App *)_app)->onRequest();
        };

        int waitForAvailableBytes(int bytes)
        {
            long timeout = millis() + 300;

            for (;;) {
                if (Wire.available() >= bytes) {
                    return true;
                }

                delay(100);

                if (millis() > timeout)
                    return false;
            }
        }

        int readByte(int &data) {

            if (!waitForAvailableBytes(1))
                return false;

            data = Wire.read();

            return true;
        };

        int readRGB(int &red, int &green, int &blue) {

            if (!waitForAvailableBytes(3))
                return false;

            red = Wire.read();
            blue = Wire.read();
            green = Wire.read();

            return true;
        };

        int onCommand(int command) {

            switch (command) {
                case CMD_INITIALIZE: {
                    int length = 0;

                    if (!readByte(length))
                        return ERR_PARAMETER_MISSING;

                    if (length < 0 || length > 240)
                        return ERR_INVALID_PARAMETER;

                    if (_strip != NULL) {
                        _strip->setColor(0, 0, 0);
                    }

                    delete _strip;
                    _strip = new NeopixelStrip(length, _neopixelPin);

                    break;
                }

                case CMD_SET_PIXEL: {

                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, index = 0;

                    if (!readByte(index))
                        return ERR_INVALID_PARAMETER;

                    if (!readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    _strip->setPixelColor(index, red, green, blue);
                    break;
                };

                case CMD_REFRESH: {

                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

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

            while (Wire.available()) {
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

        // WireIO io;
        NeopixelStrip *_strip;
        Blinker _error, _heartbeat, _busy, _extra1, _extra2;        
        uint8_t _status, _address, _neopixelPin, _stripLength;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Define a strip at the specified address and with a default length
App app(0x26, 4, 20);

void setup()
{
    app.setup();
}


void loop()
{
    app.loop();
}
