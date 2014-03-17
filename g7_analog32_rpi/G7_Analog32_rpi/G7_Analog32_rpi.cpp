/**
* @file G7_Analog32.cpp
* @author Takuya Urakawa (galileo-7.com)
* @license MIT Licence
*
* @brief This is a library for Galileo-7 I2C 32ch AnalogInputs "OOMORI" Shield.
* 	     I2C 32ch Analog-Digital Converter.
*
* @section HISTORY
* v1.0 - First release
*/

#include "G7_Analog32_rpi.h"
#include <iostream>
#include <bitset>

//Address
#define PCA9538_ADDR (0x70)
#define ADS1015_ADDR (0x48)

//Command
//IO expander
#define PCA9538_REG_OUTPUT (1)
#define PCA9538_REG_CONFIG (3)

// ADC
#define ADS1015_REG_RESULT    (0x00) // read only
#define ADS1015_REG_CONFIG    (0x01) // 2bytes
#define ADS1015_REG_LO_THRESH (0x02) // 2bytes
#define ADS1015_REG_HI_THRESH (0x03) // 2bytes

// config register
// #define ADS1015_CONFIG_HIGH_DEFAULT (B01000001)

// #define ADS1015_CONFIG_HIGH_AIN0 (B11000001)
// #define ADS1015_CONFIG_HIGH_AIN1 (B11010001)

// #define ADS1015_CONFIG_LOW (B10010011)

#define ADS1015_CONFIG_HIGH_DEFAULT (0x41)

#define ADS1015_CONFIG_HIGH_AIN0 (0xC5)//(0xC1)
#define ADS1015_CONFIG_HIGH_AIN1 (0xD5)//(0xD1)

#define ADS1015_CONFIG_LOW (0x93)

// convert raw value to voltage (mV)
#define ADC_RESULT_VOLTAGE_GAIN (1.5)

#define DIO_I2C_FD_ADDR (0)
#define ADC_I2C_FD_ADDR (1)


namespace G7_Oomori {

const uint8_t G7_Analog32::MUX_ADDR[16] = {
	0x00,0x80,0x40,0xC0,
	0x20,0xA0,0x60,0xE0,
	0x10,0x90,0x50,0xD0,
	0x30,0xB0,0x70,0xF0
};

G7_Analog32::G7_Analog32(uint8_t _addr)
: dioAddr(_addr + PCA9538_ADDR),
  adcAddr(_addr + ADS1015_ADDR),
  maxVoltage(3300)
{
};

// private methods

inline void G7_Analog32::i2cSendByte(uint8_t _addr, uint8_t _cmd, uint8_t _data){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.write(_data);
	// Wire.endTransmission();
	wiringPiI2CWriteReg8(fd[_addr], _cmd, _data);
}

inline void G7_Analog32::i2cSend2Bytes(uint8_t _addr, uint8_t _cmd, uint8_t _dataH, uint8_t _dataL){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.write(_data >> 8);
	// Wire.write(_data & 0xFF);
	// Wire.endTransmission();
	uint8_t data[2];
	data[0] = _dataH;
	data[1] = _dataL;
	// MUST BE I2C_SMBUS_BLOCK_DATA
	wiringPiI2CWriteBlock(fd[_addr], 2, _cmd, data);
}

inline uint8_t G7_Analog32::i2cReadByte(uint8_t _addr, uint8_t _cmd){
	// uint8_t ret = NULL;
	// Wire.beginTransmission(_addr);
	// Wire.write(_cmd);
	// Wire.endTransmission();
	// Wire.requestFrom(_addr, (uint8_t)1);
	// if(Wire.available()){
	// 	ret = Wire.read();
	// }
	// return ret;

	int readValue = wiringPiI2CReadReg8(fd[_addr], _cmd);
	return (uint8_t)readValue;
}

inline uint16_t G7_Analog32::i2cRead2Bytes(uint8_t _addr, uint8_t _cmd){
	// Wire.beginTransmission(i2cAddress[addr][_addr]);
	// Wire.write(_cmd);
	// Wire.endTransmission();
	// Wire.requestFrom(i2cAddress[addr][_addr], (uint8_t)2);
	// if(Wire.available()){
	// 	ret = (Wire.read() << 8) + Wire.read();
	// }

	return (uint16_t) wiringPiI2CReadReg16(fd[_addr], _cmd);
}


// public methods

int G7_Analog32::init(void){
	printf("i2c init \n");
	currentMuxAddr = 0;

	// init i2c devices
	fd[DIO_I2C_FD_ADDR] = wiringPiI2CSetup(dioAddr);
	fd[ADC_I2C_FD_ADDR] = wiringPiI2CSetup(adcAddr);
	printf("DIO : %d, ADC : %d\n", fd[DIO_I2C_FD_ADDR], fd[ADC_I2C_FD_ADDR]);

	if(fd[DIO_I2C_FD_ADDR] == -1 || fd[ADC_I2C_FD_ADDR] == -1) return -1;

	//set MUX address 0
	i2cSendByte(DIO_I2C_FD_ADDR, PCA9538_REG_OUTPUT, 0x00);
	//set IO expander all output
	i2cSendByte(DIO_I2C_FD_ADDR, PCA9538_REG_CONFIG, 0x00);
	delay(20);

	printbit((uint16_t)i2cRead2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_CONFIG));
	//set ADC standby Mode
	i2cSend2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_CONFIG, ADS1015_CONFIG_HIGH_AIN0, ADS1015_CONFIG_LOW);
	return 0;
}

uint16_t G7_Analog32::readRawValue(uint8_t _ic, uint8_t _pin){
	uint8_t reg;
	// select mux
	if(_ic == 0){
		reg = ADS1015_CONFIG_HIGH_AIN1;
		currentMuxAddr = (currentMuxAddr & 0x0F) | (MUX_ADDR[_pin]);
	}else if(_ic == 1){
		reg = ADS1015_CONFIG_HIGH_AIN0;
		currentMuxAddr = (currentMuxAddr & 0xF0) | (MUX_ADDR[_pin] >> 4);
	}else{
		return -1;
	}
	i2cSendByte(DIO_I2C_FD_ADDR, PCA9538_REG_OUTPUT, currentMuxAddr);
	delay(1);
	// select adc mux and start convert
	i2cSend2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_CONFIG, reg, ADS1015_CONFIG_LOW);

	uint16_t read = (uint16_t)i2cRead2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_CONFIG);
	while(read & 0x8000 == 0x8000){
		read = (uint16_t)i2cRead2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_CONFIG);
		delay(1);
	}
	return i2cRead2Bytes(ADC_I2C_FD_ADDR, ADS1015_REG_RESULT) >> 4;
}

// 0100000110010011
// 1101000110010011
uint16_t G7_Analog32::readVoltage(uint8_t _ic, uint8_t _pin){
	return readRawValue(_ic, _pin) * ADC_RESULT_VOLTAGE_GAIN;
}

uint16_t G7_Analog32::analogRead(uint8_t _ic, uint8_t _pin){
	return (int16_t)((float)readVoltage(_ic, _pin)/(float)maxVoltage*1024.0);
}

void G7_Analog32::setMaxVoltage(uint16_t _mv){
	maxVoltage = _mv;
}

uint16_t G7_Analog32::getMaxVoltage(){
	return maxVoltage;
}

void G7_Analog32::printbit(uint16_t _num){
	std::bitset<16> bits(_num);
	std::cout << bits << std::endl;
}

} // namespace end