/**
* @file G7_Analog32.h
* @author Takuya Urakawa (galileo-7.com)
* @license MIT Licence
*
* @brief This is a library for Galileo-7 I2C 32ch Analog-Inputs "OOMORI" Shield.
* 	     I2C 32ch Analog-Digital Converter.
*
* @section HISTORY
* v1.0 - First release
*/

#ifndef G7_ANALOG32_H
#define G7_ANALOG32_H

#include <wiringPi.h>
#include "wiringPiI2CMod.h"
#include <stdio.h>

/*===========================================================================*/
#define AIN_A 0 
#define AIN_B 1
/*===========================================================================*/

namespace G7_Oomori {

#ifndef G7_ARDUINO_COMPATIBLE
	#define G7_ARDUINO_COMPATIBLE
	typedef unsigned char uint8_t;
	typedef unsigned short uint16_t;
	typedef signed short int16_t;
#endif

class G7_Analog32{
public:
	G7_Analog32(uint8_t _addr = 0);

	int init(void);
	uint16_t readRawValue(uint8_t _ic, uint8_t _pin);
	uint16_t readVoltage(uint8_t _ic, uint8_t _pin);
	uint16_t analogRead(uint8_t _ic, uint8_t _pin);
	void setMaxVoltage(uint16_t _mv);
	uint16_t getMaxVoltage();

private:
	static const uint8_t MUX_ADDR[16];

	uint8_t dioAddr;
	uint8_t adcAddr;
	int fd[2]; // fd[0]:DIO , f[1]:ADC

	uint8_t currentMuxAddr;

	uint16_t maxVoltage;

	inline void i2cSendByte(uint8_t _addr, uint8_t _cmd, uint8_t _data);
	inline void i2cSend2Bytes(uint8_t _addr, uint8_t _cmd, uint8_t _dataH, uint8_t _dataL);
	inline uint8_t i2cReadByte(uint8_t _addr, uint8_t _cmd);
	inline uint16_t i2cRead2Bytes(uint8_t _addr, uint8_t _cmd);

	void printbit(uint16_t _num);
};
} // namespace end

#endif