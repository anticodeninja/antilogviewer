#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <QtGlobal>
#include <QHash>

#include "log_item.h"
#include "chain_elements/chain_element.h"

class QGridLayout;
class QLineEdit;
class QUdpSocket;

class UdpSocket : public ChainElement
{
public:
    UdpSocket();
    virtual ~UdpSocket();

    virtual const char* name() {
        return "Udp Socket";
    }

    virtual void createUI(QGridLayout* layout);

private:
    quint16 _listenPort;
    QUdpSocket* _socket;
    QHash<QString, LogLevel> _levels;
};

#endif // UDP_SOCKET_H
