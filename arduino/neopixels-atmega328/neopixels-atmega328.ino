#include "neopixel-strip.h"
#include "io.h"
#include "indicators.h"

#ifdef __AVR_ATtiny85__
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 1;
const int APP_STRIP_LENGTH = 4;
#else
const int APP_I2C_ADDRESS  = 0x26;
const int APP_NEOPIXEL_PIN = 4;
const int APP_STRIP_LENGTH = 20;
#endif


class App;


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




class App {

    public:

        App() {

            _strip = NULL;
            _status = ACK;
            _loop = 0;


        }

        void setup() {
            io.begin(APP_I2C_ADDRESS);
            indicators.begin();


            if (APP_STRIP_LENGTH > 0) {
                _strip = new NeopixelStrip(APP_STRIP_LENGTH, APP_NEOPIXEL_PIN);
                _strip->setColor(0, 0, 4);
            }
        }


        void loop() {
            _loop++;

            if ((_loop % 100) == 0) {
                indicators.heartbeat();
            } 


            if (io.available()) {
                indicators.busy(true);
                _status = NAK;

                int command = 0, error = ERR_INVALID_PARAMETER;

                if (io.readByte(command))
                    error = onCommand(command);

                _status = ACK;
                indicators.busy(false);
            }
            else {
                indicators.busy(true);
                io.write(_status);
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
        int _status;
        uint32_t _loop;
};



static App app ; //= new App();
//APP_I2C_ADDRESS, APP_NEOPIXEL_PIN, APP_STRIP_LENGTH);

void setup()
{
    app.setup();
}


void loop()
{
    app.loop();
}
