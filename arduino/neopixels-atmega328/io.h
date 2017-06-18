#ifndef __IO_H
#define __IO_H

#ifdef __AVR_ATtiny85__
#include <TinyWireS.h>
#else
#include <Wire.h>
#endif




class IO {

    public:


         
#ifdef __AVR_ATtiny85__
		static inline void begin(int address) {
			TinyWireS.begin(address);
		};
      

        static void onReceive( void (*function)(uint8_t) ) {
            TinyWireS.onReceive(function);
        };
        
        static void onRequest(  void (*function)()) {
            TinyWireS.onRequest(function);            
        };
		
		static inline int read() {
			return TinyWireS.receive();
		};
      
		static inline void write(uint8_t byte) {
			TinyWireS.send(byte);
		};
      
		static inline int available() {
			return TinyWireS.available();
		};

        static void idle() {
            TinyWireS_stop_check();
        }


#else
		static inline void begin(int address) {
			Wire.begin(address);
            Wire.onReceive(IO::receive);
            Wire.onRequest(IO::request);            
		}
		static inline int read() {
			return Wire.read();
		};
		static inline void write(uint8_t byte) {
			Wire.write(byte);
		}
		static inline int available() {
			return Wire.available();
		}

       static void idle() {
       }

#endif

        static int waitForAvailableByte()
        {
            for (int i = 0; i < 10; i++) {
                if (available()) {
                    return true;
                }

                delay(10);
            }

            return false;
        }

        static int readByte(int &data) {

            if (!waitForAvailableByte())
                return false;

            data = read();

            return true;
        };

        static int readRGB(int &red, int &green, int &blue) {
            return readByte(red) && readByte(green) && readByte(blue);
        };



};

#endif
