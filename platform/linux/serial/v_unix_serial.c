//
// Created by vuonglk on 29/11/2024.
//
#include "stdlib.h"
#include "v_unix_serial.h"
#include <unistd.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h>
#include <sys/select.h>
#include <err.h>
#include <errno.h>
#include "stdio.h"

typedef struct{
    v_serial_t* m_base;
    int32_t m_baud;
    int32_t m_fd;
}unix_serial_impl_t;

static int32_t serial_config(int32_t _fd, int32_t _baud_rate){
    if( _fd < 0){
        return -1;
    }
    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(_fd, &tty) != 0) {
        return -1;
    }

    tty.c_cflag &= ~(PARENB | PARODD); // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= (CREAD | CLOCAL); // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    switch (_baud_rate) {
        case 9600:
            cfsetispeed(&tty, B9600);
            cfsetospeed(&tty, B9600);
            break;
        case 19200:
            cfsetispeed(&tty, B19200);
            cfsetospeed(&tty, B19200);
            break;
        case 38400:
            cfsetispeed(&tty, B38400);
            cfsetospeed(&tty, B38400);
            break;
        case 57600:
            cfsetispeed(&tty, B57600);
            cfsetospeed(&tty, B57600);
            break;
        case 115200:
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
            break;
        case 230400:
            cfsetispeed(&tty, B230400);
            cfsetospeed(&tty, B230400);
            break;
        case 460800:
            cfsetispeed(&tty, B460800);
            cfsetospeed(&tty, B460800);
            break;
        case 921600:
            cfsetispeed(&tty, B921600);
            cfsetospeed(&tty, B921600);
            break;
        default:
            cfsetispeed(&tty, B115200);
            cfsetospeed(&tty, B115200);
            break;
    }

    // Save tty settings, also checking for error
    if (tcsetattr( _fd, TCSANOW, &tty) != 0) {
        return -1;
    }
    printf("Setup serial %d baud rate: %d\n", _fd, _baud_rate);

    return _fd;
}

static int32_t unix_serial_open(v_serial_t* _this, uint32_t _baud){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }

    return serial_config(this->m_fd, _baud);
}

static int32_t unix_serial_set_baud(v_serial_t* _this, uint32_t _baud){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }

    return 0;
}

static int32_t unix_serial_is_open(v_serial_t* _this){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }

    return this->m_fd > 0;
}

static uint32_t unix_serial_bytes_available(v_serial_t* _this){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return 0;
    }
    int bytes = 0;
    if(ioctl(this->m_fd, FIONREAD, &bytes) < 0){
        return 0;
    }
    return bytes;
}

static int32_t unix_flush(v_serial_t* _this, SERIAL_DIRECTION _dir){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }
    if(this->m_fd > 0) {
        if(_dir == SERIAL_TX){
            tcflush(this->m_fd , TCIFLUSH);
        }else if(_dir == SERIAL_RX){
            tcflush(this->m_fd , TCOFLUSH);
        }else{
            tcflush(this->m_fd , TCIOFLUSH);
        }
    }
    return 0;
}

static int32_t unix_serial_send(v_serial_t* _this, char* _data, uint32_t _len){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }
    if(this->m_fd > 0){
        return (int32_t)write(this->m_fd, _data, _len);
    }
    return -1;
}

static int32_t unix_serial_read(v_serial_t* _this, char* _buff, uint32_t _max_len){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }
    if(this->m_fd > 0){
        return (int32_t)read( this->m_fd, _buff, _max_len);
    }
    return 0;
}

static int32_t unix_serial_read_blocking(v_serial_t* _this, char* _buff, uint32_t _max_len, uint32_t _timeout){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }

    if(this->m_fd < 0){
        return -1;
    }
    fd_set set;
    struct timeval timeout = {
            .tv_sec = _timeout/1000,
            .tv_usec = (_timeout%1000)*1000
    };
    FD_ZERO(&set);       //clear the set
    FD_SET(this->m_fd, &set);   //add our file descriptor to the set

    int ret = select(this->m_fd + 1, &set, NULL, NULL, &timeout);
    if(ret > 0){
        return (int32_t)read( this->m_fd, _buff, _max_len );
    }
    return ret;
}

static int32_t unix_serial_close(v_serial_t* _this){
    unix_serial_impl_t* this = (unix_serial_impl_t*)_this;
    if(!this){
        return -1;
    }
    close(this->m_fd);
    free(this);
    return 0;
}

static v_serial_t g_proc = {
        .send = unix_serial_send,
        .read = unix_serial_read,
        .bytes_available = unix_serial_bytes_available,
        .read_blocking = unix_serial_read_blocking,
        .flush = unix_flush,
        .close = NULL,
        .open = unix_serial_open,
        .set_baud = unix_serial_set_baud,
        .is_open = unix_serial_is_open
};

v_serial_t* unix_serial_create(const char* _port, uint32_t _baud, UNIX_SERIAL_MODE _mode){
    unix_serial_impl_t* this = malloc(sizeof(unix_serial_impl_t));
    if(!this){
        return NULL;
    }

    int serial_port = -1;

    if(_mode == UNIX_SERIAL_MODE_BLOCKING){
        serial_port = open(_port, O_RDWR | O_NOCTTY ); //| O_NDELAY
    }else{
        serial_port = open(_port, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC);
    }

    if(serial_port < 0){
        printf("Could NOT open serial device file: %s\n", _port);
        return NULL;
    }

    if(unix_serial_open(this->m_base, _baud) < 0){
        free(this);
        printf("Could NOT open init device file: %s\n", _port);
        return NULL;
    }
    this->m_base = &g_proc;
    return this->m_base;
}
