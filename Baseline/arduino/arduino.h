/*
 arduino.h - Interfaces a C program running on the host computer to
 an Arduino using Arduino API like functions

 Copyright (C) 2012 Fabio Varesano <fvaresano@yahoo.it>

 Development of this code has been supported by iCarnegie/Carniegie Mellon University
 during the development of the course materials for a introductory C programming course.
 Thanks to the course admins for allowing me to release this code as libre software.


 This program is free software: you can redistribute it and/or modify
 it under the terms of the version 3 GNU General Public License as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"

#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1

#define DIGITAL_READ 0x80
#define DIGITAL_WRITE_L 0xA0
#define DIGITAL_WRITE_H 0xB0
#define PIN_MODE_IN 0x00
#define PIN_MODE_OUT 0x10
#define ANALOG_WRITE 0x20
#define ANALOG_READ 0x30

#define DIGITAL_PIN_COUNT 13

// APIs
void initArduino(char *);
void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
int analogRead(int pin);
void analogWrite(int pin, int value);
void delay(int ms);

// utils
int validDigital(int pin);
int validPWM(int pin);
int validAnalog(int pin);
void printAllInput();

#endif

