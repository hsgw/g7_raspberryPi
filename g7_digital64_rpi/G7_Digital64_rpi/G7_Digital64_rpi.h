/*!
 * @file G7_Digital64.h
 * @author Takuya Urakawa (galileo-7.com)
 * @license MIT Licence
 * 
 * @brief This is library for Galileo-7 I2C 64Digital-IOs "OOMORI" Shield.
 * 
 * 
 * @section HISTORY
 * v1.0 - First release
*/

#ifndef G7_DIGITAL64_H
#define G7_DIGITAL64_H

#include <wiringPiI2C.h>

/*===========================================================================*/

#define DIO_A (0)
#define DIO_B (1)
#define DIO_C (2)
#define DIO_D (3)

namespace G7_Oomori{

#ifndef G7_ARDUINO_COMPATIBLE
	#define G7_ARDUINO_COMPATIBLE
	typedef unsigned char uint8_t;
	typedef unsigned short uint16_t;
	typedef signed short int16_t;
#endif

/*===========================================================================*/

class G7_Digital64 {
	public:

		G7_Digital64(uint8_t _addr = 0);

		// if I2CSetup failed, return -1
		int init(void);

		void setConfig(uint8_t _ic, uint8_t _port, uint8_t _data);
		void setConfigAll(uint8_t _ic, uint16_t _data);
		void setPolarity(uint8_t _ic, uint8_t _port, uint8_t _data);
		void portWrite(uint8_t _ic, uint8_t _port, uint8_t _data);
		void portWriteAll(uint8_t _ic, uint16_t _data);
		uint8_t portRead(uint8_t _ic, uint8_t _port);
		uint16_t portReadAll(uint8_t _ic);
		void pinMode(uint8_t _ic, uint8_t _port, uint8_t _pin, uint8_t _value);
	    void pinMode(uint8_t _ic, uint8_t _pin, uint8_t _value);
		void digitalWrite(uint8_t _ic, uint8_t _port, uint8_t _pin, uint8_t _value);
		void digitalWrite(uint8_t _ic, uint8_t _pin, uint8_t _value);
		uint8_t digitalRead(uint8_t _ic, uint8_t _port, uint8_t _pin);
		uint8_t digitalRead(uint8_t _ic, uint8_t _pin);


	private:
		uint8_t addr;
		static const uint8_t i2cAddress[2][4];
		int fd[4]; // raspberry pi file descriptions
		uint8_t output[4][2];
		uint8_t config[4][2];

		inline void i2cSend(uint8_t _addr, uint8_t _cmd, uint8_t _data);
		inline void i2cSend2Bytes(uint8_t _addr, uint8_t _cmd, uint16_t _data);
		inline uint16_t i2cRead2Bytes(uint8_t _addr, uint8_t _cmd);
		inline void calcPin(uint8_t* _port, uint8_t _pin);
};

} // namespace end

#endif
