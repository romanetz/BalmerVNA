#ifndef VNACOMMANDS_H
#define VNACOMMANDS_H

#include <QObject>
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
protected:
    USB_COMMANDS _command;
    uint32_t _waitTime;//msec
};

class VnaCommandNone : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
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

class VnaCommands : public QObject
{
    Q_OBJECT
public:

public:
    explicit VnaCommands(VnaDevice* device, QObject *parent = 0);
    ~VnaCommands();

    void addCommand(VnaCommand* command, bool autostart=true);
    void startCommand(bool wait=true);
/*
    void sendNone();
    void sendBigData(uint16_t imin, uint16_t imax);
    void sendSetFreq(uint32_t freq, uint32_t level=200);
    void sendStartSampling();
    void sendSamplingComplete();
    void sendSamplingBufferSize();
    void sendGetSamples();
*/

    uint32_t currentFreq() const { return _currentFreq; }
    void setCurrentFreq(uint32_t freq) { _currentFreq = freq; }

    bool samplingStarted() const { return _samplingStarted; }

    void commandSampling(uint32_t freq);
signals:
    void signalBadPacket();
    void signalNoneComplete();
    void signalSetFreq(uint32_t freq);
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
    void setSamplingStarted(bool b) { _samplingStarted = b;}
protected:
    void onReceiveBadPacket(const QByteArray& data);
    void printDebug(const QByteArray& data);

protected:
    //Какие команды мы уже послали (что-бы проверять на верность ответы)
    QList<VnaCommand*> commandQueue;

    int numTryBad;
    bool _debugRaw;
    bool _badPacket;

    uint32_t _currentFreq;

    bool _samplingStarted;
};

extern VnaCommands* g_commands;

#endif // VNACOMMANDS_H
