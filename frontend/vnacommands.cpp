#include "vnacommands.h"

#include "vnadevice.h"

#include <QDebug>

VnaCommands* g_commands = nullptr;


VnaCommands::VnaCommands(VnaDevice *device, QObject *parent)
    : QObject(parent)
    , device(device)
    , numTryBad(0)
    , _debugRaw(false)
    , _badPacket(false)
{
    g_commands = this;
    connect(device, SIGNAL(signalPacket(const QByteArray&)), this, SLOT(onPacket(const QByteArray&)));
}

VnaCommands::~VnaCommands()
{
    g_commands = nullptr;
}

void VnaCommands::onPacket(const QByteArray& data)
{
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

    if(commandQueue.size()==0)
    {
        onReceiveBadPacket(data);
        return;
    }

    VnaCommand* cmd = commandQueue[0];
    commandQueue.removeFirst();

    if(cmd->command()!=command)
    {
        onReceiveBadPacket(data);
        return;
    }


    cmd->onPacket(command, cdata, csize);
    delete cmd;

    if(_badPacket)
    {
        _badPacket = false;
        _debugRaw = false;
        onReceiveBadPacket(data);
    }

    if(_debugRaw)
    {
        _debugRaw = false;
        printDebug(data);
    }

    numTryBad = 0;


/*
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
*/
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

void VnaCommands::addCommand(VnaCommand* command)
{
    commandQueue.append(command);
    if(commandQueue.size()==1)
    {
        command->start();
    }
}
/*
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
*/
void VnaCommands::onReceiveBadPacket(const QByteArray& data)
{
    //Тут надо очистить всю очередь команд и както восстановить работу системы.
    qDebug() << "Bad packet received!";
    printDebug(data);

    for(VnaCommand* cmd : commandQueue)
    {
        delete cmd;
    }
    commandQueue.clear();

    numTryBad++;


    if(numTryBad<5)
    {
        addCommand(new VnaCommandNone());
    } else
    {
        emit signalBadPacket();
    }
}

void VnaCommands::debugRaw()
{
    _debugRaw = true;
}

void VnaCommands::badPacket()
{
    _badPacket = true;
}

///////////////////////////////////////////////////////////////////////////////////
VnaCommand::VnaCommand()
{
    _command = COMMAND_BAD;
}

VnaCommand::~VnaCommand()
{

}

USB_COMMANDS VnaCommand::command()
{
    return _command;
}

void VnaCommand::startCommand(USB_COMMANDS command)
{
    _command = command;
    g_commands->device->startCommand(command);
}

void VnaCommand::endCommand()
{
    g_commands->device->endCommand();
}

void VnaCommand::add(const uint8_t* data, uint32_t size)
{
    g_commands->device->add(data, size);
}

void VnaCommand::add8(uint8_t data)
{
    g_commands->device->add8(data);
}

void VnaCommand::add16(uint16_t data)
{
    g_commands->device->add16(data);
}

void VnaCommand::add32(uint32_t data)
{
    g_commands->device->add32(data);
}


void VnaCommandNone::start()
{
    startCommand(COMMAND_NONE);
    add8(0xD1);
    endCommand();
}

void VnaCommandNone::onPacket(USB_COMMANDS command, uint8_t* cdata, int csize)
{
    (void)command;

    g_commands->debugRaw();

    if(csize!=1 || cdata[0]!=1)
    {
        g_commands->badPacket();
        return;
    }

    emit g_commands->signalNoneComplete();
}

VnaCommandBigData::VnaCommandBigData(uint16_t imin, uint16_t imax)
    : imin(imin)
    , imax(imax)
{

}

void VnaCommandBigData::start()
{
    startCommand(COMMAND_BIG_DATA);
    add16(imin);
    add16(imax);
    endCommand();
}

void VnaCommandBigData::onPacket(USB_COMMANDS command, uint8_t* cdata, int csize)
{
    (void)command;
    (void)cdata;
    qDebug() << "Command 'Big data' size="<<csize;
    g_commands->debugRaw();
}

/*
 * Не забыть про!!!!
    if(command==COMMAND_START_SAMPLING)
    {
        //На эту команду не приходит никаких данных в ответ.
        return;
    }
 */
