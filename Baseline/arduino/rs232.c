/*
 ***************************************************************************
 * rs232.c - a simple cross-platform RS232 serial C library.
 * 
 * This version of the library has been based upon the original code by
 * Teunis van Beelen (see http://www.teuniz.net/RS-232/index.html ) and 
 * later simplyfied and extended to support also Macs by Fabio Varesano.
 * 
 * Copyright (C) 2005-2009 Teunis van Beelen <teuniz at gmail dot com>
 * Copyright (C) 2012 Fabio Varesano <fvaresano at yahoo dot it>
 *
 ***************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ***************************************************************************
 *
 * This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 ***************************************************************************
 */

#include "rs232.h"
#include <errno.h>

#if defined(__linux__) || defined(__APPLE__)  /* Linux and Macs (probably any other POSIX compilant OS) */

int fileDescriptor,
error;

struct termios new_port_settings,
old_port_settings;

void dump_termio(struct termios *t) {
	printf("cflag=%x\niflag=%x\noflag=%x\nlflag=%x\n",
			(unsigned int) t->c_cflag, (unsigned int)t->c_iflag,
			(unsigned int)t->c_oflag, (unsigned int)t->c_lflag);
//	int i;
//	for (i=0; i<NCCS; i++) {
//		printf("cc[%d]=%x\n", i, t->c_cc[i]);
//	}
	printf("ispeed=%x\nospeed=%x\n", (unsigned int)t->c_ispeed, (unsigned int)t->c_ospeed);
}

int OpenComport(char * port, int baudrate)
{
	int baudr;

	switch(baudrate)
	{
		case 50 : baudr = B50;
		break;
		case 75 : baudr = B75;
		break;
		case 110 : baudr = B110;
		break;
		case 134 : baudr = B134;
		break;
		case 150 : baudr = B150;
		break;
		case 200 : baudr = B200;
		break;
		case 300 : baudr = B300;
		break;
		case 600 : baudr = B600;
		break;
		case 1200 : baudr = B1200;
		break;
		case 1800 : baudr = B1800;
		break;
		case 2400 : baudr = B2400;
		break;
		case 4800 : baudr = B4800;
		break;
		case 9600 : baudr = B9600;
		break;
		case 19200 : baudr = B19200;
		break;
		case 38400 : baudr = B38400;
		break;
		case 57600 : baudr = B57600;
		break;
		case 115200 : baudr = B115200;
		break;
		case 230400 : baudr = B230400;
		break;
		/*
		 case  460800 : baudr = B460800;
		 break;
		 case  500000 : baudr = B500000;
		 break;
		 case  576000 : baudr = B576000;
		 break;
		 case  921600 : baudr = B921600;
		 break;
		 case 1000000 : baudr = B1000000;
		 break;
		 */
		default : printf("invalid baudrate\n");
		return(1);
		break;
	}

	errno = 0;
	fileDescriptor = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fileDescriptor == -1)
	{
		char str[50];
		sprintf(str, "unable to open com port, errno=%d ", errno);
		perror(str);
		return(1);
	}

	errno = 0;
	error = tcgetattr(fileDescriptor, &old_port_settings);
	if(error == -1)
	{
		close(fileDescriptor);
		char str[50];
		sprintf(str, "unable to read port settings, errno=%d ", errno);
		perror(str);
		return(1);
	}

#if defined(__APPLE__)
//  printf("OLD Termios\n");
//  dump_termio(&old_port_settings);
//  printf("baud %x\n", baudr);

//  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */
	new_port_settings = old_port_settings;
	new_port_settings.c_cflag = CS8 | CLOCAL | CREAD;
	new_port_settings.c_iflag = IGNPAR;
	new_port_settings.c_oflag = 0;
	new_port_settings.c_lflag = 0;
	new_port_settings.c_ispeed = baudr;
	new_port_settings.c_ospeed = baudr;
	int i;
	for (i=0; i<NCCS; i++) {
		new_port_settings.c_cc[i] = 0;
	}
//  printf("NEW Termios\n");
//  dump_termio(&new_port_settings);
#elif defined(__linux__)
	memset(&new_port_settings, 0, sizeof(new_port_settings)); /* clear the new struct */

	new_port_settings.c_cflag = baudr | CS8 | CLOCAL | CREAD;
	new_port_settings.c_iflag = IGNPAR;
	new_port_settings.c_oflag = 0;
	new_port_settings.c_lflag = 0;
	new_port_settings.c_cc[VMIN] = 0; /* block untill n bytes are received */
	new_port_settings.c_cc[VTIME] = 0; /* block untill a timer expires (n * 100 mSec.) */

#endif

	errno = 0;
	error = tcsetattr(fileDescriptor, TCSANOW, &new_port_settings);
	if(error == -1)
	{
		close(fileDescriptor);
		char str[50];
		sprintf(str, "unable to adjust port settings, errno=%d ", errno);
		perror(str);
		return(1);
	}

	return(0);
}

int PollComport(unsigned char *buf, int size)
{
	int n;

#ifndef __STRICT_ANSI__                       /* __STRICT_ANSI__ is defined when the -ansi option is used for gcc */
	if(size>SSIZE_MAX) size = (int)SSIZE_MAX; /* SSIZE_MAX is defined in limits.h */
#else
	if(size>4096) size = 4096;
#endif

	//printf("%d\n", fileDescriptor);
	n = read(fileDescriptor, buf, size);
	buf[n] = 0;
	//printf("fd: %d n: %d\n", fileDescriptor, n);

	return (n);
}

unsigned char PollByte() {
	unsigned char c;
	while(read(fileDescriptor, &c, sizeof(char)) != 1); //cycle until we read a byte
	return c;
}

int SendByte(unsigned char byte)
{
	int n;

	errno = 0;
	n = write(fileDescriptor, &byte, 1);
	if(n<0) {
		char str[50];
		sprintf(str, "SendByte: write failed, errno=%d ", errno);
		perror(str);
		return(1);
	}

	return(0);
}

int SendBuf(unsigned char *buf, int size)
{
	return write(fileDescriptor, buf, size);
}

void CloseComport()
{
	close(fileDescriptor);
	tcsetattr(fileDescriptor, TCSANOW, &old_port_settings);
}

#else         /* windows */

HANDLE Cport;

/*
 char comports[16][10]={"\\\\.\\COM1",  "\\\\.\\COM2",  "\\\\.\\COM3",  "\\\\.\\COM4",
 "\\\\.\\COM5",  "\\\\.\\COM6",  "\\\\.\\COM7",  "\\\\.\\COM8",
 "\\\\.\\COM9",  "\\\\.\\COM10", "\\\\.\\COM11", "\\\\.\\COM12",
 "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15", "\\\\.\\COM16"};
 */
char baudr[64];

int OpenComport(char * port_name, int baudrate) {
	// Serial device names in Win needs the "\\\\.\\" prefix. 
	// A Serial device is named as eg: \\\\.\\COM1
	char port[30] = "\\\\.\\";
	strcat(port, port_name);

	switch (baudrate) {
	case 110:
		strcpy(baudr, "baud=110 data=8 parity=N stop=1");
		break;
	case 300:
		strcpy(baudr, "baud=300 data=8 parity=N stop=1");
		break;
	case 600:
		strcpy(baudr, "baud=600 data=8 parity=N stop=1");
		break;
	case 1200:
		strcpy(baudr, "baud=1200 data=8 parity=N stop=1");
		break;
	case 2400:
		strcpy(baudr, "baud=2400 data=8 parity=N stop=1");
		break;
	case 4800:
		strcpy(baudr, "baud=4800 data=8 parity=N stop=1");
		break;
	case 9600:
		strcpy(baudr, "baud=9600 data=8 parity=N stop=1");
		break;
	case 19200:
		strcpy(baudr, "baud=19200 data=8 parity=N stop=1");
		break;
	case 38400:
		strcpy(baudr, "baud=38400 data=8 parity=N stop=1");
		break;
	case 57600:
		strcpy(baudr, "baud=57600 data=8 parity=N stop=1");
		break;
	case 115200:
		strcpy(baudr, "baud=115200 data=8 parity=N stop=1");
		break;
	case 128000:
		strcpy(baudr, "baud=128000 data=8 parity=N stop=1");
		break;
	case 256000:
		strcpy(baudr, "baud=256000 data=8 parity=N stop=1");
		break;
	default:
		printf("invalid baudrate\n");
		return (1);
		break;
	}

	Cport = CreateFileA(port, GENERIC_READ | GENERIC_WRITE, 0, /* no share  */
	NULL, /* no security */
	OPEN_EXISTING, 0, /* no threads */
	NULL); /* no templates */

	if (Cport == INVALID_HANDLE_VALUE ) {
		printf("unable to open comport\n");
		return (1);
	}

	DCB port_settings;
	memset(&port_settings, 0, sizeof(port_settings)); /* clear the new struct  */
	port_settings.DCBlength = sizeof(port_settings);

	if (!BuildCommDCBA(baudr, &port_settings)) {
		printf("unable to set comport dcb settings\n");
		CloseHandle(Cport);
		return (1);
	}

	if (!SetCommState(Cport, &port_settings)) {
		printf("unable to set comport cfg settings\n");
		CloseHandle(Cport);
		return (1);
	}

	COMMTIMEOUTS Cptimeouts;

	Cptimeouts.ReadIntervalTimeout = 50;
	Cptimeouts.ReadTotalTimeoutMultiplier = 0;
	Cptimeouts.ReadTotalTimeoutConstant = 0;
	Cptimeouts.WriteTotalTimeoutMultiplier = 0;
	Cptimeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(Cport, &Cptimeouts)) {
		printf("unable to set comport time-out settings\n");
		CloseHandle(Cport);
		return (1);
	}

	return (0);
}

int PollComport(unsigned char *buf, int size) {
	int n;

	if (size > 4096)
		size = 4096;

	/* added the void pointer cast, otherwise gcc will complain about */
	/* "warning: dereferencing type-punned pointer will break strict aliasing rules" */

	ReadFile(Cport, buf, size, (LPDWORD) ((void *) &n), NULL);

	return (n);
}

int PollResponse(char *buffRead) {
		DWORD dwBytesRead = 0;
		int nread = 256;

		while (dwBytesRead == 0) {
			if (!ReadFile(Cport, buffRead, nread, &dwBytesRead, NULL )) {
				return 0;
			}
		}
		return (int)dwBytesRead;
}

unsigned char PollByte() {
	unsigned char c;
	int n;

	ReadFile(Cport, &c, 1, (LPDWORD) ((void *) &n), NULL);
	return c;
}

int SendByte(unsigned char byte) {
	int n;

	WriteFile(Cport, &byte, 1, (LPDWORD) ((void *) &n), NULL);

	if (n < 0)
		return (1);

	return (0);
}

int SendBuf(unsigned char *buf, int size) {
	int n;

	if (WriteFile(Cport, buf, size, (LPDWORD) ((void *) &n), NULL)) {
		return (n);
	}

	return (-1);
}

void CloseComport() {
	CloseHandle(Cport);
}

#endif

