#ifndef KIJANGCLIENT_H
#define KIJANGCLIENT_H

#include <QObject>
#include <QLoggingCategory>
#include <QDebug>
#include <QTcpSocket>
#include <QDir>
#include <QRandomGenerator>
#include "kijangprotocol.h"

class KijangClient : public QObject
{
    Q_OBJECT
public:
    explicit KijangClient(QObject *parent = nullptr);
    ~KijangClient();
    void setPort(quint16 newPort);

protected:
    bool internalConnectionSet;
    bool readyRead;
    QString m_errorString;
    QString m_address;
    quint16 m_port;
    QTcpSocket socket;

signals:
    void connectedChanged();
    void attemptingConnectionChanged();

protected slots:
    void clientConnected();
    void clientDisconnected();
    void clientError(QAbstractSocket::SocketError socketError);
    void clientStateChanged(QAbstractSocket::SocketState socketState);
    void clientReadyRead();

};

#endif // KIJANGCLIENT_H
