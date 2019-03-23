#include "udp_socket.h"

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QXmlStreamReader>

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

UdpSocket::UdpSocket()
    : _listenPort(7071)
    , _levels({
        { "TRACE", LogLevel::TRACE },
        { "DEBUG", LogLevel::DEBUG },
        { "INFO", LogLevel::INFO },
        { "WARN", LogLevel::WARN },
        { "ERROR", LogLevel::ERROR },
        { "FATAL", LogLevel::FATAL },
    })
{
    _socket = new QUdpSocket();
    _socket->bind(QHostAddress::LocalHost, _listenPort);
    _socket->connect(_socket, &QUdpSocket::readyRead, [this]{
        while (_socket->hasPendingDatagrams())
        {
            auto datagram = _socket->receiveDatagram();
            auto logItem = std::make_shared<LogItem>();
            logItem->Type = LogItemType::Log;

            QXmlStreamReader xml(datagram.data());
            xml.setNamespaceProcessing(false);

            while (xml.readNextStartElement()) {
                if (xml.name() == "event") {
                    logItem->Source = xml.attributes().value("logger").toString();
                    logItem->Timestamp = xml.attributes().value("timestamp").toDouble() * 1000.0;
                    logItem->Level = _levels[xml.attributes().value("level").toString()];
                } else if (xml.name() == "message") {
                    logItem->Message = xml.readElementText();
                }
            }

            if (!xml.hasError()) {
                ChainElement::accept(logItem);
            } else {
                qWarning() << xml.errorString();
            }
        }
    });
}

void UdpSocket::createUI(QGridLayout* layout) {
    auto ctrListenPort = new QSpinBox();
    ctrListenPort->setMinimum(0x0000);
    ctrListenPort->setMaximum(0xFFFF);
    ctrListenPort->setValue(_listenPort);

    auto ctrApply = new QPushButton("Apply");
    ctrApply->connect(ctrApply, &QPushButton::clicked, [this, ctrListenPort] {
        _listenPort = ctrListenPort->value();
        _socket->abort();
        _socket->bind(QHostAddress::LocalHost, _listenPort);
    });

    layout->addWidget(new QLabel("Listen Port:"), 1, 0);
    layout->addWidget(ctrListenPort, 1, 1);
    layout->addWidget(ctrApply, 2, 0, 1, 2);
}
