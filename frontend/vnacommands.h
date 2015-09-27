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
signals:

public slots:
    void onPacket(const QByteArray& data);
protected:
    VnaDevice* device;
};

#endif // VNACOMMANDS_H
