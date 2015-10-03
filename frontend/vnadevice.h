#ifndef VNADEVICE_H
#define VNADEVICE_H

#include <QtSerialPort/QSerialPort>

const int VIRTUAL_COM_PORT_DATA_SIZE = 64;

class VnaDevice : public QObject
{
    Q_OBJECT
public:
    VnaDevice();

    bool open();

    void startCommand(uint8_t command);
    void endCommand();
    //Вызывать тольбко внутри startCommand/endCommand
    void add(const uint8_t* data, uint32_t size);
    void add8(uint8_t data);
    void add16(uint16_t data);
    void add32(uint32_t data);

    void printDebug(const QByteArray& data);
signals:
    //Пришел пакет с девайса
    void signalPacket(const QByteArray& data);
    void signalError(QSerialPort::SerialPortError error);
    void signalClose();
public slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();
protected:
    void closeSerialPort();
protected:
    QSerialPort *serial;
    QByteArray sendBuffer;
    QByteArray readBuffer;

    bool commandStarted;
    bool lastIsFE;
};

#endif // VNADEVICE_H
