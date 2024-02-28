#include "Uart.h"


bool uartInit(int &serialPort)
{
    // 打开串口
    serialPort = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPort < 0) {
        std::cerr << "Error opening serial port" << std::endl;
        return false;
    }

    // 配置串口
    struct termios tty;
    if(tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Error from tcgetattr" << std::endl;
        return false;
    }
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);
    tty.c_oflag &= ~OPOST;
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr" << std::endl;
        return false;
    }
    return true;
}


void sendData(int serialPort, unsigned char* data, int len)
{
    write(serialPort, data, len);
}