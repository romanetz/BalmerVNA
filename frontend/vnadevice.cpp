#include "vnadevice.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "mainwindow.h"

VnaDevice::VnaDevice()
    : commandStarted(false)
    , lastIsFE(false)
{
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

bool VnaDevice::open()
{
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
    {
        qDebug() << "serial name=" << info.portName() << " desc=" << info.description() << " busy=" << info.isBusy();
    }

    QString serialName = "ttyACM0";
    serial->setPortName(serialName);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (!serial->open(QIODevice::ReadWrite))
    {
        QMessageBox::critical(mainWindow, tr("Error"), serial->errorString());
        return false;
    }

    qDebug() << "Serial port connected:" << serialName;

    lastIsFE = false;
    return true;
}

void VnaDevice::closeSerialPort()
{
    if (serial->isOpen())
    {
        serial->close();
        emit signalClose();
    }
}


void VnaDevice::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(mainWindow, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }

    emit signalError(error);
}

void VnaDevice::readData()
{
    QByteArray data = serial->readAll();

    for(uint8_t c : data)
    {
        if(c==0xFF)
        {
            emit signalPacket(readBuffer);
            readBuffer.clear();
            continue;
        }

        if(c==0xFE)
        {
            lastIsFE = true;
            continue;
        }

        if(lastIsFE)
        {
            lastIsFE = false;
            if(c==0)
                c = 0xFE;
            else
            if(c==1)
                c = 0xFF;
            else
            {
                Q_ASSERT(0);
            }
        }

        readBuffer.append(c);
    }

}

void VnaDevice::add(const uint8_t* data, uint32_t size)
{
    Q_ASSERT(commandStarted);
    for(uint32_t i=0; i<size; i++)
    {
        if(data[i]>=0xFE)
        {
            sendBuffer.append(0xFE);
            sendBuffer.append(data[i]-0xFE);
        } else
        {
            sendBuffer.append(data[i]);
        }
    }
}

void VnaDevice::add8(uint8_t data)
{
    add(&data, sizeof(data));
}

void VnaDevice::add16(uint16_t data)
{
    add((uint8_t*)&data, sizeof(data));
}

void VnaDevice::add32(uint32_t data)
{
    add((uint8_t*)&data, sizeof(data));
}


void VnaDevice::startCommand(uint8_t command)
{
    commandStarted = true;
    sendBuffer.append(command);
}

void VnaDevice::endCommand()
{
    sendBuffer.append(0xFF);
    serial->write(sendBuffer);
    sendBuffer.clear();
    commandStarted = false;
}


