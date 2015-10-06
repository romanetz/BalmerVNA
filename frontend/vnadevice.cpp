#include "vnadevice.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "mainwindow.h"
#include "../4code/inc/commands.h"

VnaDevice::VnaDevice()
    : commandStarted(false)
    , lastIsFE(false)
{
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

bool VnaDevice::open()
{
    QString serialName;

    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
    {
        if(serialName.isEmpty())
            serialName = info.portName();
        qDebug() << "serial name=" << info.portName() << " desc=" << info.description() << " busy=" << info.isBusy();
    }

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

void VnaDevice::printDebug(const QByteArray& data)
{
    //debug commands
    QString strDebug;
    for(uint8_t c : data)
    {
        strDebug += QString::number(c, 16);
        strDebug += " ";
    }
    qDebug() << "Receive:" << strDebug;
}

void VnaDevice::update()
{
    //if(!serial->isOpen())
    //    return;
    //startCommand(COMMAND_EMPTY_ANSVER);
    //endCommand();
    //qDebug() << "empty";
/*
    qint64 bytes = serial->bytesAvailable();
    if(bytes!=0)
    {
        qDebug() << "bytesAvailable=" << bytes;
    }
*/
}


void VnaDevice::readData()
{
    QByteArray data = serial->readAll();
    //printDebug(data);

    for(uint8_t c : data)
    {
        if(c==0xFF)
        {
            if(readBuffer.size()>0)
            {
                emit signalPacket(readBuffer);
            } else
            {
                qDebug() << "empty";
            }
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
    serial->flush();
    serial->write(sendBuffer);

    sendBuffer.clear();
    commandStarted = false;
}


