/*
 ***************************************************************************
 * rs232.h - a simple cross-platform RS232 serial C library.
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

#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#else

#include <windows.h>

#endif

int OpenComport(char *, int);
int PollComport(unsigned char *, int);
int PollResponse(char *buffRead);
unsigned char PollByte();
int SendByte(unsigned char);
int SendBuf(unsigned char *, int);
void CloseComport();
void cprintf(const char *);
int IsCTSEnabled();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

