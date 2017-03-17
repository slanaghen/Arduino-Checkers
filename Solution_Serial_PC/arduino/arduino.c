/*
 arduino.c - Interfaces a C program running on the host computer to
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

#include "arduino.h"

unsigned char debug[512];

/****
 * Initialize the Arduino
 *
 * in: port - the port to use to talk to the arduino (e.g. COM3 or /dev/ttyACM0)
 * out: none
 * return: none
 ****/
void initArduino(char * port) {
	int bdrate = 115200; // port baud

	if (OpenComport(port, bdrate)) {
		printf(
				"Impossible to establish connection.\nCheck your connection. Exiting.\n");
		exit(1);
	}

	delay(2000);

	PollComport(debug, 512); // clean input buffer
	//printf("%s\n", debug);
}

/****
 * Designate a pin for input (from the arduino) or output (to the arduino).  Does
 * nothing if the pin is not valid. Valid pins are 2 to DIGITAL_PIN_COUNT, inclusive.
 *
 * in:	pin - which pin to assign a mode to
 * 		mode - one of INPUT or OUTPUT
 * out: none
 * return: none
 ****/
void pinMode(unsigned char pin, unsigned char mode) {
	if (!validDigital(pin)) {
		return;
	}

	if (mode == INPUT) {
		SendByte(PIN_MODE_IN | pin);
	} else if (mode == OUTPUT) {
		SendByte(PIN_MODE_OUT | pin);
	}
	//printAllInput();
}

/****
 * Write to the designated pin.  Does nothing if the specified pin is not valid.
 * Valid pins are 2 to DIGITAL_PIN_COUNT, inclusive.
 *
 * in:	pin - which pin to write to
 * 		value - one of HIGH or LOW - the value to write to the pin
 * out: none
 * return: none
 ****/
void digitalWrite(int pin, int value) {
	if (!validDigital(pin)) {
		return;
	}

	if (value == LOW) {
		SendByte(DIGITAL_WRITE_L | pin);
	} else if (value == HIGH) {
		SendByte(DIGITAL_WRITE_H | pin);
	}
	//printAllInput();
}

/****
 * Read from the designated pin. Valid pins are 2 to DIGITAL_PIN_COUNT, inclusive.
 *
 * in:	pin - which pin to read from
 * out: none
 * return: value read from pin.  Returns -1 if the pin is not a valid pin.
 ****/
int digitalRead(int pin) {
	if (!validDigital(pin)) {
		return -1;
	}

	SendByte(DIGITAL_READ | pin); // send digital read message

	unsigned int res = PollByte(); // poll serial line until arduino replies
	if ((res & 0xf0) == DIGITAL_READ) { // check for correct data
		return res & 0x0f; // return result
	}
	return -1; // error condition
}

/****
 * Write to the designated pin.  Does nothing if pin not valid.
 * Valid pins are 3, 5, 6, 9, 10, and 11.  Value written is undefined if
 * not between 0 and 1023.
 *
 * in:	pin - which pin to read from
 * 		value - value to write to the pin
 * out: none
 * return: none
 ****/
void analogWrite(int pin, int value) {
	if (!validPWM(pin)) {
		return;
	}

	SendByte(ANALOG_WRITE | pin);
	SendByte(value);
	//printAllInput();
}

/****
 * Read from the designated pin.  Valid pins are 0 - 5, inclusive.
 *
 * in:	pin - which pin to read from
 * out: none
 * return: value read from pin.  Returns -1 if the pin is not a valid pin.
 ****/
int analogRead(int pin) {
	if (!validAnalog(pin)) {
		return -1;
	}

	SendByte(ANALOG_READ | pin);

	unsigned int res1 = PollByte();
	if ((res1 & 0xf0) == ANALOG_READ) { // check for correct data
		unsigned int res2 = PollByte();
		return ((res1 & 0x03) << 8) | res2;
	}
	return -1;
}

// UTILS

// valid pins for digitial read
int validDigital(int pin) {
	return (2 <= pin && pin <= DIGITAL_PIN_COUNT);
}

// valid pins for analog write
int validPWM(int pin) {
	return (pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10
			|| pin == 11);
}

// valid pins for analog read
int validAnalog(int pin) {
	return (0 <= pin && pin <= 5);
}

void printAllInput() {
	PollComport(debug, 512);
	printf("%s\n", debug);
}

void delay(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

