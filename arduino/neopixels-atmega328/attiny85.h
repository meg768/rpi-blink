
class App {

    
    public:

        App(int address, int neopixelPin, int stripLength) {
            _app = (void *)this;

            _strip = NULL;
            _status = ACK;
            _address = address;
            _stripLength = stripLength;
            _neopixelPin = neopixelPin;

            _heartbeat.setPin(3);
            _error.setPin(4);

        }

        void setup() {
            TinyWireS.begin(_address);


            _heartbeat.blink(2, 250);
            _error.blink(2, 250);

             _strip = new NeopixelStrip(_stripLength, _neopixelPin);
             _strip->setColor(0, 0, 4);
        }


        void loop() {
            static uint32_t counter = 0;

            counter++;

            if ((counter % 100) == 0) {
                _heartbeat.toggleState();
            } 

            if (_status != 0) {
               TinyWireS.send(_status);
               _status = 0;
            }

            else if (TinyWireS.available()) {
                int command = -1, error = ERR_INVALID_COMMAND;

                if (readByte(command)) {
                    error = onCommand(command);
                }

                if (error > 0) {
                    _status = NAK;
                    _error.blink(error);
                }
                else {
                    _status = ACK;
                }
                

            }

            delay(1);

        };

        int flush()
        {
            long timeout = millis() + 300;

            for (;;) {
                if (!TinyWireS.available()) {
                    break;
                }

                TinyWireS.receive();
                delay(100);

            }
        }


        int waitForAvailableBytes(int bytes)
        {
            for (int i = 0; i < 10; i++) {
                if (TinyWireS.available() >= bytes) {
                    return true;
                }

                delay(100);
            }
            
            return false;
        }

        int readByte(int &data) {

            if (!waitForAvailableBytes(1))
                return false;

            data = TinyWireS.receive();

            return true;
        };

        int readRGB(int &red, int &green, int &blue) {


            if (!readByte(red))
                return false;
            if (!readByte(green))
                return false;
            if (!readByte(blue))
                return false;
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

                    //_strip->setColor(128, 128, 0);
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




    private:

        NeopixelStrip *_strip;
        Blinker _heartbeat, _error;
        uint8_t _status, _address, _neopixelPin, _stripLength;
};
