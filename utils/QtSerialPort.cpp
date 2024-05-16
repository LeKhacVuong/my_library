//
// Created by vuonglk on 13/09/2023.
//

#include "QtSerialPort.h"
#include "Logger.h"
#include <QTime>
#include "algorithm"
#include "QDebug"
#include "Timer.h"



#include "linux_serial.h"

#define TAG "SerialPort"

SerialPort::SerialPort(const std::string &_devFile, int32_t _baudRate, bool _isBlocking) {
    m_devFile = _devFile;
    m_baudRate = _baudRate;
    m_blocking = _isBlocking;
    m_serial = std::make_shared<QSerialPort>();
    m_serial->setPortName((const QString)m_devFile.c_str());
    m_serial->setBaudRate(m_baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

}

SerialPort::~SerialPort() {
    m_blocking = false;
    m_baudRate = -1;
    m_devFile.clear();
    m_serial->close();
}

int32_t SerialPort::open(int32_t _baudRate) {
    m_baudRate = _baudRate;
    m_serial->setBaudRate(m_baudRate);
    if(!m_serial->open(QSerialPort::ReadWrite)){
        LOG_INFO(TAG, "Open SerialPort %s %s", m_devFile.c_str(), "FAILED");
        return -1;
    }
    if (!m_serial->flush()) {
        return -1; // Nếu không flush được, trả về -1
    }
    LOG_INFO(TAG, "Open SerialPort %s %s", m_devFile.c_str(), "SUCCESS");
    return 0;
}

int32_t SerialPort::close(){

    LOG_INFO(TAG, "Close Serial Port %s", m_devFile.c_str());
    m_serial->close();
    return -1;
}

bool SerialPort::isOpen() const {
    return m_serial->isOpen();
}

int32_t SerialPort::config(int32_t _baudRate) {
    m_baudRate = _baudRate;
    m_serial->setBaudRate(m_baudRate);
    return 0;
}

int32_t SerialPort::getBaudRate() const {
    return m_baudRate;
}

int32_t SerialPort::dataIsAvailable() const {
    return m_serial->bytesAvailable();
}

void SerialPort::flush() const {
     m_serial->flush();

}

int32_t SerialPort::send(const uint8_t *_data, int32_t _len, int32_t _timeout) const {
    if (!m_serial->isOpen() || !m_serial->isWritable()) {
        return -1;
    }

    int32_t bytesWritten = m_serial->write(reinterpret_cast<const char*>(_data), _len);

    if (!m_serial->waitForBytesWritten(_timeout + 1)) {
        qint64 bytesRemaining = m_serial->bytesToWrite();
        if (bytesRemaining > 0) {
        qDebug()<< "bytesRemaining is: "<<bytesRemaining;
        }
        return -1;
    }

    return bytesWritten;
}


int32_t SerialPort::read(uint8_t* _buf, int32_t _len, int32_t _timeout){
    if (!m_serial->isOpen() || !m_serial->isReadable()) {
        return -1;
    }
    WaitTimer timeout(_timeout + 1);
    QByteArray data;

    while (m_serial->bytesAvailable() < _len && timeout.getRemainTime()) {
       m_serial->waitForReadyRead(10);
    }

    if (m_serial->bytesAvailable() >= _len) {
        data = m_serial->read(_len);
        if(data.size() < _len) return -1;
        std::copy(data.begin(), data.end(), _buf);
        return data.size();
    }

    return -1;
}

