#include "vnacommands.h"

#include "vnadevice.h"

#include "../4code/inc/job.h"

#include <QDebug>
#include <QTimer>

VnaCommands* g_commands = nullptr;

static bool com_debug = false;

uint32_t g_pingIdx = 12;

VnaCommands::VnaCommands(VnaDevice *device, QObject *parent)
    : QObject(parent)
    , device(device)
    , numTryBad(0)
    , _debugRaw(false)
    , _badPacket(false)
    , _restartCurrentCommand(false)
    , _currentFreq(0)
    , _samplingStarted(false)
    , _samplingBufferSize(0)
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

    VnaCommand* cmd = commandQueue[0];

    if(cmd->firstByteIsCommand())
    {
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


        if(cmd->command()!=command)
        {
            onReceiveBadPacket(data);
            return;
        }

        cmd->onPacket(cdata, csize);
    } else
    {
        cmd->onPacket(cdata, csize);
    }

    if(!_restartCurrentCommand)
    {
        commandQueue.removeFirst();
        delete cmd;
    }

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

    _restartCurrentCommand = false;

    startCommand();
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

void VnaCommands::appendCommand(VnaCommand* command, bool autostart)
{
    commandQueue.append(command);
    if(autostart && commandQueue.size()==1)
    {
        startCommand();
    }
}

void VnaCommands::startCommand(bool wait)
{
    while(!commandQueue.empty())
    {
        VnaCommand* cmd = commandQueue.first();

        if(wait)
        if(cmd->waitBeforeStart()>0)
        {
            QTimer::singleShot(cmd->waitBeforeStart(), this, SLOT(onWaitStart()));
            break;
        }

        wait = true;

        cmd->start();
        if(cmd->command()!=COMMAND_BAD)
        {
            if(com_debug)
                qDebug() << "Start command " << cmd->command();
            break;
        }


        commandQueue.removeFirst();
        delete cmd;
    }
}

void VnaCommands::onWaitStart()
{
    if(com_debug)
        qDebug() << "onWaitStart()";
    startCommand(false);
}

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

    emit signalBadPacket();
}

void VnaCommands::debugRaw()
{
    _debugRaw = true;
}

void VnaCommands::badPacket()
{
    _badPacket = true;
}

void VnaCommands::setSamplingBufferSize(uint16_t size)
{
    _samplingBufferSize = size;
    _arrayI.resize(_samplingBufferSize);
    _arrayQ.resize(_samplingBufferSize);

    for(int i=0; i<_samplingBufferSize; i++)
    {
        _arrayI[i] = 0;
        _arrayQ[i] = 0;
    }
}

void VnaCommands::commandInitial()
{
    appendCommand(new VnaCommandPing());
    appendCommand(new VnaCommandPing());
    appendCommand(new VnaCommandSamplingBufferSize(), false);
    appendCommand(new VnaCommandPing());
    appendCommand(new VnaCommandSetTx(false));
}

void VnaCommands::commandSampling(uint32_t freq)
{
    appendCommand(new VnaCommandSetFreq(freq), false);
    appendCommand(new VnaCommandStartSampling(), false);
    appendCommand(new VnaCommandSamplingComplete(10), false);

    for(int sampleQ = 0; sampleQ<2; sampleQ++)
    {
        int count = 250; //Т.к. буфер на девайсе у нас 2048 и надо оставить запас на конвертацию FE и FF
        for(int offset=0; offset<_samplingBufferSize; offset+=count)
        {
            appendCommand(new VnaCommandGetSamples(sampleQ?true:false, offset, qMin(count, _samplingBufferSize-offset)), false);
        }
    }

    appendCommand(new VnaCommandEndSampling(), false);

    startCommand();
}

void VnaCommands::setHardData(const HardSamplingData& data)
{
    _hardSamplingData = data;
}

///////////////////////////////////////////////////////////////////////////////////
VnaCommand::VnaCommand()
{
    _command = COMMAND_BAD;
    _waitTime = 0;
}

VnaCommand::~VnaCommand()
{

}

USB_COMMANDS VnaCommand::command()
{
    return _command;
}

uint32_t VnaCommand::waitBeforeStart()
{
    return _waitTime;
}

void VnaCommand::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    (void)csize;
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


void VnaCommandPing::start()
{
    pingIdx = g_pingIdx++;
    startCommand(COMMAND_PING);
    add32(pingIdx);
    endCommand();
}

void VnaCommandPing::onPacket(uint8_t* cdata, int csize)
{
    if(com_debug)
        g_commands->debugRaw();

    if(csize!=4 || pingIdx!=*(uint32_t*)cdata)
    {
        g_commands->badPacket();
        return;
    }
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

void VnaCommandBigData::onPacket(uint8_t* cdata, int csize)
{
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

VnaCommandSetFreq::VnaCommandSetFreq(uint32_t freq, uint32_t level)
    : freq(freq)
    , level(level)
{

}

void VnaCommandSetFreq::start()
{
    startCommand(COMMAND_SET_FREQ);
    add32(freq);
    add32(level);
    endCommand();
}

void VnaCommandSetFreq::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=4);
    uint32_t freq = *(uint32_t*)cdata;
    if(com_debug)
        qDebug() << "COM: Set freq" << freq;
    g_commands->setCurrentFreq(freq);
}

void VnaCommandStartSampling::start()
{
    startCommand(COMMAND_START_SAMPLING);
    endCommand();

    _command = COMMAND_BAD; //В ответ на эту команду не приходит никаких данных.
    g_commands->setSamplingStarted(true);
}


VnaCommandSamplingComplete::VnaCommandSamplingComplete(int retryCount)
    : retryCount(retryCount)
{
    _waitTime = 10;
}

void VnaCommandSamplingComplete::start()
{
    startCommand(COMMAND_SAMPLING_COMPLETE);
    endCommand();
}

void VnaCommandSamplingComplete::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=1);
    bool complete = cdata[0]?true:false;
    if(com_debug)
        qDebug() << "COM: Sampling complete" << complete;

    if(!complete)
    {
        if(retryCount>0)
        {
            retryCount--;
            g_commands->restartCurrentCommand();
        }
    } else
    {
        g_commands->setSamplingStarted(false);
    }
}

void VnaCommandSamplingBufferSize::start()
{
    startCommand(COMMAND_SAMPLING_BUFFER_SIZE);
    endCommand();
}

void VnaCommandSamplingBufferSize::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=2);
    uint16_t bufferSize = *(uint16_t*)cdata;
    if(com_debug)
        qDebug() << "COM: Sampling buffer size=" << bufferSize;
    g_commands->setSamplingBufferSize(bufferSize);
}

VnaCommandGetSamples::VnaCommandGetSamples(bool sampleQ, uint16_t offset, uint16_t count)
    : sampleQ(sampleQ)
    , offset(offset)
    , count(count)
{

}

void VnaCommandGetSamples::start()
{
    startCommand(COMMAND_GET_SAMPLES);
    add8(sampleQ?1:0);
    add16(offset);
    add16(count);
    endCommand();
}

void VnaCommandGetSamples::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==count*4);
    int32_t* inData = (int32_t*)cdata;

    if(com_debug)
        qDebug() << "COM: Receive samples "<< (sampleQ?"Q":"I") <<" " <<offset << "," << count;

    QVector<int32_t>& v = sampleQ?g_commands->_arrayQ:g_commands->_arrayI;
    //QString s;

    for(int i=0; i<count; i++)
    {
        v[offset+i] = inData[i];
        //s += " " + QString::number(inData[i]);
    }

    //qDebug() << "samples=" << s;
}

void VnaCommandEndSampling::start()
{
    emit g_commands->signalEndSampling();
}

VnaCommandSetTx::VnaCommandSetTx(bool tx)
    : tx(tx)
{

}

void VnaCommandSetTx::start()
{
    startCommand(COMMAND_SET_TX);
    add8(tx?1:0);
    endCommand();

}

void VnaCommandSetTx::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==1);
    if(com_debug)
        qDebug() << "tx=" << cdata[0];
}

VnaCommandStartSamplingAndCalculate::VnaCommandStartSamplingAndCalculate()
{
    _waitTime = 10;
}

void VnaCommandStartSamplingAndCalculate::start()
{
    startCommand(COMMAND_START_SAMPLING_AND_CALCULATE);
    endCommand();
}

void VnaCommandStartSamplingAndCalculate::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    (void)csize;
    //qDebug() << "StartSamplingAndCalculate";
}

VnaCommandGetCalculated::VnaCommandGetCalculated(int retryCount)
    : retryCount(retryCount)
{
    _waitTime = 10;
}

void VnaCommandGetCalculated::start()
{
    startCommand(COMMAND_GET_CALCULATED);
    endCommand();
}

void VnaCommandGetCalculated::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=1);
    uint8_t jobState = cdata[0];
    if(com_debug)
        qDebug() << "COM: Sampling job " << jobState;

    if(jobState!=JOB_CALCULATING_COMPLETE)
    {
        if(retryCount>0)
        {
            retryCount--;
            g_commands->restartCurrentCommand();
        }
    } else
    {
        Q_ASSERT(csize==1+sizeof(HardSamplingData));

        g_commands->setHardData(*(HardSamplingData*)(cdata+1));
        emit g_commands->signalReceiveHard();
    }

}
