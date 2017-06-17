#include "neopixel-strip.h"
#include "io.h"
#include "indicators.h"

#ifdef __AVR_ATtiny85__
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 1;
const int APP_STRIP_LENGTH = 8;
#else
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 4;
const int APP_STRIP_LENGTH = 20;
#endif


class App;

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

static App *_app = NULL;


class App {

    public:

        App() {
            _app = this;

            _strip = NULL;
            _status = ERR_OK;
            _loop = 0;


        }

        void setup() {
            io.begin(APP_I2C_ADDRESS);
            indicators.begin();

             _strip = new NeopixelStrip(APP_STRIP_LENGTH, APP_NEOPIXEL_PIN);
             _strip->setColor(0, 0, 4);
        }


        void loop() {
            _loop++;

            if ((_loop % 100) == 0) {
                indicators.heartbeat();
            } 

            if (_status != ERR_OK) {
               io.write(_status == 0 ? ACK : NAK);

               indicators.error(_status);
                
               _status = ERR_OK;
            }

            else if (io.available()) {
                indicators.busy(true);
                
                int command = -1;
                
                if (io.readByte(command)) {
                    _status = onCommand(command);
                }
                else {
                    _status = ERR_INVALID_PARAMETER;
                }
                
                indicators.busy(false);

            }

            delay(1);

        };


        int onCommand(int command) {

            switch (command) {
                case CMD_INITIALIZE: {
                    int length = 0;

                    if (!io.readByte(length))
                        return ERR_PARAMETER_MISSING;

                    if (length < 0 || length > 240)
                        return ERR_INVALID_PARAMETER;

                    if (_strip != NULL) {
                        _strip->setColor(0, 0, 0);
                    }

                    delete _strip;
                    _strip = new NeopixelStrip(length, APP_NEOPIXEL_PIN);

                    break;
                }

                case CMD_SET_PIXEL: {

                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, index = 0;

                    if (!io.readByte(index))
                        return ERR_INVALID_PARAMETER;

                    if (!io.readRGB(red, green, blue))
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

                    if (!io.readRGB(red, green, blue))
                      return ERR_INVALID_PARAMETER;

                    //_strip->setColor(128, 128, 0);
                    _strip->setColor(red, green, blue);

                    break;
                };

                case CMD_WIPE_TO_COLOR: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, delay = 0;

                    if (!io.readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    if (!io.readByte(delay))
                        return ERR_INVALID_PARAMETER;

                    _strip->wipeToColor(red, green, blue, delay);

                    break;
                }

                case CMD_FADE_TO_COLOR: {
                    if (_strip == NULL)
                        return ERR_NOT_INITIALIZED;

                    int red = 0, green = 0, blue = 0, steps = 0;

                    if (!io.readRGB(red, green, blue))
                        return ERR_INVALID_PARAMETER;

                    if (!io.readByte(steps))
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


    private:

        IO io;
        NeopixelStrip *_strip;
        Indicators indicators;
        uint8_t _status;
        uint32_t _loop;
};




void setup()
{
    _app = new App();
    _app->setup();
}


void loop()
{
    _app->loop();
}
