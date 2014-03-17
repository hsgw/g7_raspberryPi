#makefile for wiringpi

TARGET = analog32_rpi_example
LIBS = G7_Analog32_rpi/G7_Analog32_rpi.o G7_Analog32_rpi/wiringPiI2CMod.o

SRCS = $(shell ls *.cpp)
OBJS = $(SRCS:.cpp=.o)

CFLAG = -Wall

############################################
CC = g++
LIBPATH =  -I/usr/local/include -L/usr/local/lib -lwiringPi

ALL : $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $(CFLAG) $(LIBPATH) $^

.cpp.o:
	$(CC) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(LIBS)