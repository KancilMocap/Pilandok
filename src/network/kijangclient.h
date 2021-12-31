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
    void connectToServer();
    void disconnectFromServer();
    virtual void onConnected() = 0;

    bool connected() const;
    bool attemptingConnection() const;

signals:
    void connectedChanged(bool newConnected);
    void attemptingConnectionChanged(bool newAttemptingConnection);

protected:
    void setConnected(bool newConnected);
    void setAttemptingConnection(bool newAttemptingConnection);

    bool m_connected;
    bool m_attemptingConnection;
    bool internalConnectionSet;
    bool readyRead;
    QString m_errorString;
    QString m_address;
    quint16 m_port;
    QTcpSocket socket;

protected slots:
    void clientConnected();
    void clientDisconnected();
    void clientStateChanged(QAbstractSocket::SocketState socketState);

};

#endif // KIJANGCLIENT_H
