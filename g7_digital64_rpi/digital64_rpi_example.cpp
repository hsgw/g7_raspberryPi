#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>

#include "G7_Digital64_rpi/G7_Digital64_rpi.h"

G7_Oomori::G7_Digital64 dio;

int main(void){
	if(wiringPiSetup()){
		printf("wiriingPi setup error!\n");
		return 0;
	}
	printf("wiringPi setup done");

	if(dio.init() == -1) {
		printf("i2c setup error!\n");
		return 0;
	}
	printf("i2c setup done\n");

	//pin setup
	dio.setConfig(DIO_A, 0, 0x00);
	dio.setConfig(DIO_A, 1, 0x00);
	
	char pinState = 0x55;

	while(1){
		dio.portWrite(DIO_A, 0, pinState);
		delay(500);
		pinState = ~pinState;
		dio.portWrite(DIO_A, 0, pinState);
		delay(500);
	}

	return 0;
}
