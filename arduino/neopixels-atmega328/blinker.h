

class Blinker {

    public:

        Blinker(int pin = -1) {

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
