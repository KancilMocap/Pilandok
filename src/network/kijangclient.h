#ifndef KIJANGCLIENT_H
#define KIJANGCLIENT_H

#include "kijangprotocol.h"
#include <QObject>
#include <QLoggingCategory>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include "kijangservercontrolmodule.h"

class KijangClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool attemptingConnection READ attemptingConnection WRITE setAttemptingConnection NOTIFY attemptingConnectionChanged)
    Q_PROPERTY(QString buttonString READ buttonString WRITE setButtonString NOTIFY buttonStringChanged)
public:
    explicit KijangClient(QObject *parent = nullptr);
    Q_INVOKABLE void toggleConnection();
    void connectToServer();
    void disconnectFromServer();
    void asyncSendRequest(KijangProtocol request);

    const QString &address() const;
    void setAddress(const QString &newAddress);

    quint16 port() const;
    void setPort(quint16 newPort);

    bool connected() const;
    void setConnected(bool newConnected);

    bool attemptingConnection() const;
    void setAttemptingConnection(bool newAttemptingConnection);

    const QString &buttonString() const;
    void setButtonString(const QString &newButtonString);

    const QString &serverName() const;
    const QString &serverVersion() const;

private:
    bool internalConnectionSet;
    bool m_connected;
    bool m_attemptingConnection;
    bool waitingForDisconnectReceived;
    bool readyRead;
    QString m_errorString;
    QString m_address;
    QString m_buttonString;
    quint16 m_port;
    quint32 m_clientID;

    QString m_serverName;
    QString m_serverVersion;

    QTcpSocket socket;
    void onResponseReceived(KijangProtocol response); // Can be sync or async
    void initClientID();

    KijangServerControlModule moduleFFFE;

signals:
    void responseReceived(quint64 requestID, KijangProtocol response);
    void connectedChanged();
    void addressChanged();
    void portChanged();
    void attemptingConnectionChanged();
    void buttonStringChanged();

public slots:
    void moduleSendRequest(KijangProtocol request);
    void moduleSetClientID(quint32 id);
    void modulePongReceived();
    void moduleServerName(QString name);
    void moduleServerVersion(QString version);
    void moduleServerRequestDisconnect();

private slots:
    void clientConnected();
    void clientDisconnected();
    void clientError(QAbstractSocket::SocketError socketError);
    void clientStateChanged(QAbstractSocket::SocketState socketState);
    void clientReadyRead();

};

#endif // KIJANGCLIENT_H
