#include "vnacommands.h"

#include "vnadevice.h"

#include <QDebug>

#include "../4code/inc/commands.h"


VnaCommands::VnaCommands(VnaDevice *device, QObject *parent)
    : QObject(parent)
    , device(device)
{
    connect(device, SIGNAL(onPacket(const QByteArray&)), this, SLOT(onPacket(const QByteArray&)));
}

void VnaCommands::onPacket(const QByteArray& data)
{
    QString strDebug;

    for(uint8_t c : data)
    {
        strDebug += QString::number(c, 16);
        strDebug += " ";
    }

    qDebug() << "Receive:" << strDebug;

}

void VnaCommands::sendNone()
{
    device->startCommand(COMMAND_NONE);
    device->add8(0xD1);
    device->endCommand();
}

void VnaCommands::sendBigData(uint16_t imin, uint16_t imax)
{
    device->startCommand(COMMAND_BIG_DATA);
    device->add16(imin);
    device->add16(imax);
    device->endCommand();

}
