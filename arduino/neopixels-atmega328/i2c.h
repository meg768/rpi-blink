
class I2C {

    public:
        I2C(int address) {
        };

        ~I2C() {
        };

		void begin(int address) {
			Wire.begin(_address);
            Wire.onReceive(App::receive);
            Wire.onRequest(App::request);

		}
		int available() {

		};

		int read() {

		};

		void write(int8_t byte) {

		};



    private:

};
