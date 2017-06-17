
#include "io.h"
#include "indicators.h"

class App {

    
    public:

        App(int address, int neopixelPin, int stripLength) {
            _app = (void *)this;

            _strip = NULL;
            _status = ERR_OK;
            _address = address;
            _stripLength = stripLength;
            _neopixelPin = neopixelPin;


        }

        void setup() {
            io.begin(_address);
            indicators.begin();

             _strip = new NeopixelStrip(_stripLength, _neopixelPin);
             _strip->setColor(0, 0, 4);
        }


        void loop() {
            static uint32_t counter = 0;

            counter++;

            if ((counter % 100) == 0) {
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
                    _status = ERR_INVALID_COMMAND;
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
                    _strip = new NeopixelStrip(length, _neopixelPin);

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
        uint8_t _status, _address, _neopixelPin, _stripLength;
};
