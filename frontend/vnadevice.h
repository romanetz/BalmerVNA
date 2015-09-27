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

    void sendNone();
signals:

public slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();
protected:
    void closeSerialPort();

    //Вызывать тольбко внутри startCommand/endCommand
    void add(uint8_t* data, uint32_t size);
protected:
    QSerialPort *serial;
    QByteArray sendBuffer;

    bool commandStarted;
};

#endif // VNADEVICE_H
