#ifndef __UART_H__
#define __UART_H__
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <termios.h>

using namespace std;

bool uartInit(int &serialPort);
void sendData(int serialPort, unsigned char* data, int len);

#endif