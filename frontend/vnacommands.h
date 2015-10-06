#ifndef VNACOMMANDS_H
#define VNACOMMANDS_H

#include <QObject>
#include <QVector>
#include "../4code/inc/commands.h"


class VnaDevice;

class VnaCommand
{
public:
    VnaCommand();
    virtual ~VnaCommand();

    void startCommand(USB_COMMANDS command);
    void endCommand();
    void add(const uint8_t* data, uint32_t size);
    void add8(uint8_t data);
    void add16(uint16_t data);
    void add32(uint32_t data);

    virtual USB_COMMANDS command();

    //Вызывается один раз при запуске команды
    virtual void start()=0;

    virtual void onPacket(uint8_t* cdata, int csize);

    //return msec time
    virtual uint32_t waitBeforeStart();

    virtual bool firstByteIsCommand() { return true; }
protected:
    USB_COMMANDS _command;
    uint32_t _waitTime;//msec
};

class VnaCommandPing : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    uint32_t pingIdx;
};

class VnaCommandBigData : public VnaCommand
{
protected:
    uint16_t imin;
    uint16_t imax;
public:
    VnaCommandBigData(uint16_t imin, uint16_t imax);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandSetFreq : public VnaCommand
{
    uint32_t freq;
    uint32_t level;
public:
    VnaCommandSetFreq(uint32_t freq, uint32_t level=200);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandStartSampling : public VnaCommand
{
public:
    void start() override;
};

class VnaCommandSamplingComplete : public VnaCommand
{
public:
    VnaCommandSamplingComplete(int retryCount);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    int retryCount;
};

class VnaCommandSamplingBufferSize : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandGetSamples : public VnaCommand
{
public:
    VnaCommandGetSamples(bool sampleQ, uint16_t offset, uint16_t count);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

    bool firstByteIsCommand() override { return false; }
protected:
    bool sampleQ;
    uint16_t offset;
    uint16_t count;
};

class VnaCommandEndSampling : public VnaCommand
{
public:
    void start();
};

class VnaCommandSetTx : public VnaCommand
{
public:
    VnaCommandSetTx(bool tx);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    bool tx;
};

class VnaCommands : public QObject
{
    Q_OBJECT
public:

public:
    explicit VnaCommands(VnaDevice* device, QObject *parent = 0);
    ~VnaCommands();

    void appendCommand(VnaCommand* command, bool autostart=true);
    void startCommand(bool wait=true);

    uint32_t currentFreq() const { return _currentFreq; }
    void setCurrentFreq(uint32_t freq) { _currentFreq = freq; }  //вызывается только из команды

    uint16_t samplingBufferSize() { return _samplingBufferSize; }
    void setSamplingBufferSize(uint16_t size);  //вызывается только из команды

    bool samplingStarted() const { return _samplingStarted; }
    void setSamplingStarted(bool b) { _samplingStarted = b;} //вызывается только из команды

    void commandInitial();

    void commandSampling(uint32_t freq);

    QVector<int32_t>& arrayI() { return _arrayI; }
    QVector<int32_t>& arrayQ() { return _arrayQ; }

signals:
    void signalBadPacket();
    void signalSetFreq(uint32_t freq);
    void signalEndSampling();
public slots:
    void onPacket(const QByteArray& data);
    void onWaitStart();
public:
    //Для VnaCommand и ее потомков
    VnaDevice* device;
    //Вывести следующюю команду в дебаговый output
    void debugRaw();
    //Пришла плохая команда, очищаем очередь команд.
    void badPacket();
    //Перезапускаем текущую команду
    void restartCurrentCommand() { _restartCurrentCommand = true; }

protected:
    void onReceiveBadPacket(const QByteArray& data);
    void printDebug(const QByteArray& data);

protected:
    //Какие команды мы уже послали (что-бы проверять на верность ответы)
    QList<VnaCommand*> commandQueue;

    int numTryBad;
    bool _debugRaw;
    bool _badPacket;
    bool _restartCurrentCommand;

    uint32_t _currentFreq;

    bool _samplingStarted;
    uint16_t _samplingBufferSize;

    QVector<int32_t> _arrayI;
    QVector<int32_t> _arrayQ;

    friend class VnaCommandGetSamples;
};

extern VnaCommands* g_commands;

#endif // VNACOMMANDS_H
