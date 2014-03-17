/**
 * @file G7_Digital64.cpp
 * @author Takuya Urakawa (galileo-7.com)
 * @license MIT Licence
 * 
 * @brief This is a library for Galileo-7 I2C 64Digital-IOs "OOMORI" Shield.
 * 	      Control 64 digital IO via I2C.
 * 
 * @section HISTORY
 * v1.0 - First release
*/

#include "G7_Digital64_rpi.h"
#include <wiringPiI2C.h>

//PORT0 Command 
#define PCA9655_REG_INPUT    (0)
#define PCA9655_REG_OUTPUT   (2)
#define PCA9655_REG_POLARITY (4)
#define PCA9655_REG_CONFIG  (6)

namespace G7_Oomori{

const uint8_t G7_Digital64::i2cAddress[2][4] = {{0x20,0x24,0x22,0x26},{0x21,0x25,0x23,0x27}};

// constructor
G7_Digital64::G7_Digital64(uint8_t _addr)
 : addr(_addr)
{
}

// private methods

inline void G7_Digital64::i2cSend(uint8_t _addr, uint8_t _cmd, uint8_t _data){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.write(_data);
	// Wire.endTransmission();
	wiringPiI2CWriteReg8(fd[_addr], _cmd, _data);
}

inline void G7_Digital64::i2cSend2Bytes(uint8_t _addr, uint8_t _cmd, uint16_t _data){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.write(_data >> 8);
	// Wire.write(_data & 0xFF);
	// Wire.endTransmission();
	wiringPiI2CWriteReg16(fd[_addr], _cmd, _data);
}

inline uint16_t G7_Digital64::i2cRead2Bytes(uint8_t _addr, uint8_t _cmd){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.endTransmission();
	// Wire.requestFrom(i2cAddress[addr][_addr], (uint8_t)2);
	// if(Wire.available()){
	// 	ret = (Wire.read() << 8) + Wire.read();
	// }

	int readValue = wiringPiI2CReadReg16(fd[_addr], _cmd);
	return (uint16_t)readValue;
}

inline void G7_Digital64::calcPin(uint8_t* _port,uint8_t _pin){
	_port[0] = _pin >> 3;
	_port[1] = _pin - (_port[0] << 3);
}


// public methods

int G7_Digital64::init(void){
	for(uint8_t i=0; i<4;i++){
		fd[i] = wiringPiI2CSetup(i2cAddress[addr][i]);
		if(fd[i] == -1) return -1;
	}

	
	for(uint8_t i=0;i<4;i++){
		for(uint8_t j=0;j<2;j++){
			config[i][j] = 0xFF;
			output[i][j] = 0xFF;
		}
	}

	for(uint8_t ic=0;ic<4;ic++){
		for(uint8_t port=0;port<2;port++){
			setConfig(ic, port, 0xFF);
			setPolarity(ic, port, 0x00);
			portWrite(ic, port, 0xFF);
		}
	}
	return 1;
}

void G7_Digital64::setConfig(uint8_t _ic, uint8_t _port, uint8_t _data){
	config[_ic][_port] = _data;
	i2cSend(_ic, PCA9655_REG_CONFIG + _port, _data);
}

void G7_Digital64::setConfigAll(uint8_t _ic, uint16_t _data){
	config[_ic][0] = _data >> 8;
	config[_ic][1] = _data & 0xFF;
	i2cSend2Bytes(_ic, PCA9655_REG_CONFIG, _data);
}

void G7_Digital64::setPolarity(uint8_t _ic, uint8_t _port, uint8_t _data){
	i2cSend(_ic, PCA9655_REG_POLARITY + _port, _data);
}

void G7_Digital64::portWrite(uint8_t _ic, uint8_t _port, uint8_t _data){
	output[_ic][_port] = _data;
	i2cSend(_ic, PCA9655_REG_OUTPUT + _port, _data);
}

void G7_Digital64::portWriteAll(uint8_t _ic, uint16_t _data){
	output[_ic][0] = _data >> 8;
	output[_ic][1] = _data & 0xFF;
	i2cSend2Bytes(_ic, PCA9655_REG_OUTPUT, _data);
}

uint8_t G7_Digital64::portRead(uint8_t _ic, uint8_t _port){
	if(_port == 0) return (i2cRead2Bytes(_ic, PCA9655_REG_INPUT)>>8);
	else  return (i2cRead2Bytes(_ic, PCA9655_REG_INPUT) & 0xFF);
}

uint16_t G7_Digital64::portReadAll(uint8_t _ic){
	return i2cRead2Bytes(_ic, PCA9655_REG_INPUT);
}

void G7_Digital64::pinMode(uint8_t _ic, uint8_t _port, uint8_t _pin, uint8_t _value){
	uint8_t tempConfig = config[_ic][_port] & ~((uint8_t)0x01 << _pin);
	tempConfig |= ((_value == 0?1:0) << _pin);
	setConfig(_ic, _port, tempConfig);
}

void G7_Digital64::pinMode(uint8_t _ic, uint8_t _pin, uint8_t _value){
	uint8_t port[2];
	calcPin(port, _pin);
	pinMode(_ic, port[0], port[1], _value);
}

void G7_Digital64::digitalWrite(uint8_t _ic, uint8_t _port, uint8_t _pin, uint8_t _value){
	uint8_t tempOutput = output[_ic][_port] & ~(0x01 << _pin);
	tempOutput |= (_value << _pin);
	portWrite(_ic, _port, tempOutput);
}

void G7_Digital64::digitalWrite(uint8_t _ic, uint8_t _pin, uint8_t _value){
	uint8_t port[2];
	calcPin(port, _pin);
	digitalWrite(_ic, port[0], port[1], _value);
}

uint8_t G7_Digital64::digitalRead(uint8_t _ic, uint8_t _port, uint8_t _pin){
	uint16_t tempInput = portRead(_ic, _port);
	// if(tempInput == -1) return NULL;
	return ((tempInput & (0x01 << _pin)) == 0 ? 0:1);
}

uint8_t G7_Digital64::digitalRead(uint8_t _ic, uint8_t _pin){
	uint8_t port[2];
	calcPin(port, _pin);
	return digitalRead(_ic, port[0], port[1]);
}

} // namespace end
