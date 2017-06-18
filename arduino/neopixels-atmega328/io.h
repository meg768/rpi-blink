#ifndef __IO_H
#define __IO_H

#ifdef __AVR_ATtiny85__
#include <TinyWireS.h>
#else
#include <Wire.h>
#endif


class IO {

    public:
        IO() {
        };

        virtual ~IO() {
        };

#ifdef __AVR_ATtiny85__
		inline void begin(int address) {
			TinyWireS.begin(address);
		}
		inline uint8_t read() {
			return TinyWireS.receive();
		};
		inline void write(uint8_t byte) {
			TinyWireS.send(byte);
		}
		inline int available() {
			return TinyWireS.available();
		}

#else
		inline void begin(int address) {
			Wire.begin(address);
		}
		inline uint8_t read() {
			return Wire.read();
		};
		inline void write(uint8_t byte) {
			Wire.write(byte);
		}
		inline int available() {
			return Wire.available();
		}

#endif

        int waitForAvailableByte()
        {
            for (int i = 0; i < 10; i++) {
                if (available()) {
                    return true;
                }

                delay(10);
            }

            return false;
        }

        int readByte(uint8_t &data) {

            if (!waitForAvailableByte())
                return false;

            data = read();

            return true;
        };

        int readRGB(uint8_t &red, uint8_t &green, uint8_t &blue) {
            return readByte(red) && readByte(green) && readByte(blue);
        };



};

#endif

