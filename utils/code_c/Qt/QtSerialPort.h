//
// Created by vuonglkon 13/09/2023.
//

#ifndef QT_SERIALPORT_H
#define QT_SERIALPORT_H

#include <cstdint>
#include <string>
#include "unix/serial/linux_serial.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "memory"

class SerialPort {
public:
    explicit SerialPort(const std::string& _devFile, int32_t _baudRate = 115200, bool _isBlocking = false);

    ~SerialPort();

    int32_t open(int32_t _baudRate = 115200);

    int32_t close();

    bool isOpen() const;

    int32_t config(int32_t _baudRate);

    int32_t getBaudRate() const;

    int32_t dataIsAvailable() const;

    void flush() const;

    int32_t read(uint8_t* _buf, int32_t _len, int32_t _timeout);

    int32_t send(const uint8_t* _data, int32_t _len, int32_t _timeout) const;

private:
    std::shared_ptr<QSerialPort> m_serial;
    std::string m_devFile;
    bool m_blocking;
    int32_t m_baudRate;
};


#endif //QT_SERIALPORT_H
