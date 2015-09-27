#include "vnadevice.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "mainwindow.h"
#include "../4code/inc/commands.h"

VnaDevice::VnaDevice()
    : commandStarted(false)
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

    return true;
}

void VnaDevice::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
}


void VnaDevice::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(mainWindow, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void VnaDevice::readData()
{
    QByteArray data = serial->readAll();
    QString strDebug;

    for(uint8_t c : data)
    {
        strDebug += QString::number(c, 16);
        strDebug += " ";
    }

    qDebug() << "Receive:" << strDebug;
}

void VnaDevice::add(uint8_t* data, uint32_t size)
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


void VnaDevice::sendNone()
{
    startCommand(COMMAND_NONE);
    uint8_t data = 0xD1;
    add(&data, 1);
    endCommand();
}
