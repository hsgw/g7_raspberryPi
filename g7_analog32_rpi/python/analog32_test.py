#!/usr/bin/python

import smbus
import time

# CONSTANTS
PCA9538_ADDR = 0x70
ADS1015_ADDR = 0x48

PCA9538_REG_OUTPUT = 1
PCA9538_REG_CONFIG = 3

ADS1015_REG_RESULT = 0x00
ADS1015_REG_CONFIG = 0x01

ADS1015_CONFIG_DEFAULT = 0xC593

bus = smbus.SMBus(1)

# setting up dio
# set all pins to output and low
bus.write_byte_data(PCA9538_ADDR, PCA9538_REG_OUTPUT, 0)
bus.write_byte_data(PCA9538_ADDR, PCA9538_REG_CONFIG, 0)

time.sleep(0.001)

while 1:
	# conversion start
	#bus.write_word_data(ADS1015_ADDR, ADS1015_REG_CONFIG, ADS1015_CONFIG_DEFAULT)
	bus.write_i2c_block_data(ADS1015_ADDR, ADS1015_REG_CONFIG, [0xc5,0x93])

	adcConfig = bus.read_word_data(ADS1015_ADDR, ADS1015_REG_CONFIG)
	print hex(adcConfig)
	# wait for conversion end

	time.sleep(0.1)

	print("result")
	print hex(bus.read_word_data(ADS1015_ADDR, ADS1015_REG_RESULT))

	time.sleep(1.0)