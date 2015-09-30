#ifndef VNACOMMANDS_H
#define VNACOMMANDS_H

#include <QObject>
class VnaDevice;

class VnaCommands : public QObject
{
    Q_OBJECT
public:
    explicit VnaCommands(VnaDevice* device, QObject *parent = 0);

    void sendNone();
    void sendBigData(uint16_t imin, uint16_t imax);
    void sendSetFreq(uint32_t freq, uint32_t level=200);
    void sendStartSampling();
    void sendSamplingComplete();
    void sendSamplingBufferSize();
    void sendGetSamples();
signals:
    void signalBadPacket();
    void signalNoneComplete();
    void signalSetFreq(uint32_t freq);
public slots:
    void onPacket(const QByteArray& data);
protected:
    void startCommand(uint8_t command);
    void endCommand();

    void onReceiveBadPacket(const QByteArray& data);
    void printDebug(const QByteArray& data);

    void onNone(uint8_t size);
protected:
    VnaDevice* device;

    //Какие команды мы уже послали (что-бы проверять на верность ответы)
    QList<uint8_t> commandQueue;

    int numTryBad;
};

#endif // VNACOMMANDS_H
