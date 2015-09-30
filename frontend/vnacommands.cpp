#include "vnacommands.h"

#include "vnadevice.h"

#include <QDebug>

#include "../4code/inc/commands.h"


VnaCommands::VnaCommands(VnaDevice *device, QObject *parent)
    : QObject(parent)
    , device(device)
    , numTryBad(0)
{
    connect(device, SIGNAL(signalPacket(const QByteArray&)), this, SLOT(onPacket(const QByteArray&)));
}

void VnaCommands::onPacket(const QByteArray& data)
{
    bool debugRaw = true;

    uint8_t* cdata = (uint8_t*)data.data();
    int csize = data.size();

    USB_COMMANDS command;

    if(csize==0)
    {
        //Bad command
        command = (USB_COMMANDS)-1;
    } else
    {
        cdata++;
        csize--;
        command = (USB_COMMANDS)(data[0]);
    }

    if(commandQueue.size()==0 || commandQueue[0]!=command)
    {
        onReceiveBadPacket(data);
        return;
    }

    commandQueue.removeFirst();

    switch(command)
    {
    case COMMAND_NONE:
        debugRaw = true;

        if(csize!=1)
        {
            onReceiveBadPacket(data);
            return;
        }

        onNone(cdata[0]);

        break;
    case COMMAND_BIG_DATA:
        qDebug() << "Command 'Big data'";
        debugRaw = true;
        break;
    case COMMAND_SET_FREQ:
    {
        Q_ASSERT(csize>=4);
        uint32_t freq = *(uint32_t*)cdata;
        qDebug() << "COM: Set freq" << freq;
        emit signalSetFreq(freq);
    }
    break;
    case COMMAND_START_SAMPLING:
        Q_ASSERT(0);
        break;
    case COMMAND_SAMPLING_COMPLETE:
    {
        Q_ASSERT(csize>=1);
        bool complete = cdata[0]?true:false;
        qDebug() << "COM: Sampling complete" << complete;
    }
    break;
    case COMMAND_SAMPLING_BUFFER_SIZE:
    {
        Q_ASSERT(csize>=4);
        uint16_t bufferSize = *(uint16_t*)cdata;
        qDebug() << "COM: Sampling buffersize" << bufferSize;
    }
    break;
    case COMMAND_GET_SAMPLES:
    {
    }
    break;
    case COMMAND_SET_TX:
        break;
    case COMMAND_GET_CALCULATED:
        break;
    case COMMAND_START_SAMPLING_AND_CALCULATE:
        break;
    case COMMAND_CS4272_READ_REG:
        break;
    default:
        qDebug() << "Receive unknown command:" << command;
        break;
    }

    if(debugRaw)
    {
        printDebug(data);
    }
}

void VnaCommands::printDebug(const QByteArray& data)
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

void VnaCommands::startCommand(uint8_t command)
{
    device->startCommand(command);

    if(command==COMMAND_START_SAMPLING)
    {
        //На эту команду не приходит никаких данных в ответ.
        return;
    }

    commandQueue.append(command);
}

void VnaCommands::endCommand()
{
    device->endCommand();
}

void VnaCommands::sendNone()
{
    startCommand(COMMAND_NONE);
    device->add8(0xD1);
    endCommand();
}

void VnaCommands::sendBigData(uint16_t imin, uint16_t imax)
{
    startCommand(COMMAND_BIG_DATA);
    device->add16(imin);
    device->add16(imax);
    endCommand();

}

void VnaCommands::sendSetFreq(uint32_t freq, uint32_t level)
{
    startCommand(COMMAND_SET_FREQ);
    device->add32(freq);
    device->add32(level);
    endCommand();
}

void VnaCommands::sendStartSampling()
{
    startCommand(COMMAND_START_SAMPLING);
    endCommand();
}

void VnaCommands::sendSamplingComplete()
{
    startCommand(COMMAND_SAMPLING_COMPLETE);
    endCommand();
}

void VnaCommands::sendSamplingBufferSize()
{
    startCommand(COMMAND_SAMPLING_BUFFER_SIZE);
    endCommand();
}

void VnaCommands::sendGetSamples()
{
    startCommand(COMMAND_GET_SAMPLES);
    endCommand();
}

void VnaCommands::onReceiveBadPacket(const QByteArray& data)
{
    //Тут надо очистить всю очередь команд и както восстановить работу системы.
    qDebug() << "Bad packet received!";
    printDebug(data);
    commandQueue.clear();

    numTryBad++;


    if(numTryBad<5)
    {
        sendNone();
    } else
    {
        emit signalBadPacket();
    }
}

void VnaCommands::onNone(uint8_t size)
{
    qDebug() << "Command 'None' size=" << size;
    if(size==1)
    {
        numTryBad = 0;
        emit signalNoneComplete();
    }

}
