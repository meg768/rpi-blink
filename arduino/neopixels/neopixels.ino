#include "neopixel-strip.h"
#include "i2c.h"
#include "indicators.h"

#ifdef __AVR_ATtiny85__
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 1;
const int APP_STRIP_LENGTH = 8;
#else
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 4;
const int APP_STRIP_LENGTH = 40;
#endif

class App;

const int ACK = 6;
const int NAK = 21;

const int CMD_INITIALIZE    = 0x10;  // size
const int CMD_SET_COLOR     = 0x11;  // red, green, blue
const int CMD_FADE_TO_COLOR = 0x12;  // red, green, blue
const int CMD_WIPE_TO_COLOR = 0x13;  // red, green, blue, delay

const int ERR_OK                = 0;
const int ERR_INVALID_PARAMETER = 1;
const int ERR_PARAMETER_MISSING = 2;
const int ERR_NOT_INITIALIZED   = 3;
const int ERR_INVALID_COMMAND   = 4;


static App *_app = NULL;


class App {

    public:

        App(int length, int pin) : _strip(length, pin) {

            _app = this;
            _status = ACK;
            _loop = 0;
        }

        void setup() {
            io.onReceive(App::onReceiveService);
            io.onRequest(App::onRequestService);
            io.begin(APP_I2C_ADDRESS);
            
            indicators.begin();
            
            _strip.begin();
            _strip.setColor(0, 0, 128);
        }

        static void onReceiveService() {
            _app->onReceive();
        }

        static void onRequestService() {
            _app->onRequest();
        }

        void onReceive() {
            if (io.available()) {
                indicators.busy(true);
                _status = NAK;

                int command = 0, error = ERR_INVALID_PARAMETER;

                if (io.readByte(command))
                    error = onCommand(command);

                _status = ACK;
                indicators.busy(false);
            }
        }

        void onRequest() {
            io.write(_status);
        }


        void loop() {
            io.idle();
            
            _loop++;

            if ((_loop % 1000) == 0) {
                indicators.heartbeat();
            } 


        };


        int onCommand(int command) {

            switch (command) {
                case CMD_INITIALIZE: {
                    int length = 0;

                    if (!io.readByte(length))
                        return ERR_PARAMETER_MISSING;

                    if (length < 0 || length > 240)
                        return ERR_INVALID_PARAMETER;
                    
                    _strip.setColor(0, 0, 0);

                    _strip.updateLength(length);
                    _strip.setColor(16, 0, 0);

                    break;
                }

                case CMD_SET_COLOR: {

                    int index = 0, length = 0, red = 0, green = 0, blue = 0;

                    if (!io.readByte(index) || !io.readByte(length) || !io.readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    _strip.setColor(index, length, red, green, blue);

                    break;
                };

                case CMD_WIPE_TO_COLOR: {

                    int index = 0, length = 0, red = 0, green = 0, blue = 0, delay = 0;

                    if (!io.readByte(index) || !io.readByte(length) || !io.readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    if (!io.readByte(delay))
                        return ERR_INVALID_PARAMETER;

                    _strip.wipeToColor(index, length, red, green, blue, delay);

                    break;
                }

                case CMD_FADE_TO_COLOR: {

                    int index = 0, length = 0, red = 0, green = 0, blue = 0;
                    uint16_t speed = 0;
                    
                    if (!io.readByte(index) || !io.readByte(length) || !io.readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    if (!io.readWord(speed))
                        return ERR_INVALID_PARAMETER;

                    _strip.fadeToColor(index, length, red, green, blue, speed);

                    break;
                }

                default: {
                    return ERR_INVALID_COMMAND;
                }
            };

            return ERR_OK;

        }


    private:
        I2C io;
        NeopixelStrip _strip;
        Indicators indicators;
        volatile int _status;
        uint32_t _loop;
};


static App app(APP_STRIP_LENGTH, APP_NEOPIXEL_PIN);


void setup()
{
    app.setup();
}


void loop()
{
    app.loop();
}
