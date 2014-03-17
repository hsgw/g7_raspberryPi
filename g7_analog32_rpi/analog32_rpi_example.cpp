#include <wiringPi.h>
#include <stdio.h>

#include "G7_Analog32_rpi/G7_Analog32_rpi.h"

G7_Oomori::G7_Analog32 adc;

int main(void){
	if(wiringPiSetup()){
		printf("wiriingPi setup error!\n");
		return 0;
	}
	printf("wiringPi setup done\n");

	if(adc.init() == -1) {
		printf("i2c setup error!\n");
		return 0;
	}
	printf("i2c setup done\n");

	while(1){
		for(int i=0;i<8;i++){
			int value = adc.readVoltage(AIN_A,i);
			printf("%2d : %4d, ", i, value);
		}
		printf("\n\n");
		delay(1000);
	}

	return 0;
}
