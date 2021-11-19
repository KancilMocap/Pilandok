#ifndef KIJANGCLIENT_H
#define KIJANGCLIENT_H

#include <kijangprotocol.h>
#include <QObject>
#include <QLoggingCategory>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QInputDialog>
#include <QMetaObject>
#include <QDir>
#include <QSettings>
#include <cryptlib.h>
#include <blake2.h>
#include <pwdbased.h>
#include <hkdf.h>
#include <QRandomGenerator>
#include "kijangmainmodule.h"
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
    ~KijangClient();
    Q_INVOKABLE void toggleConnection();
    void connectToServer();
    void disconnectFromServer();
    void asyncSendRequest(Kijang::KijangProtocol request);
    void requestPassword(quint16 authType, QByteArray authDetails, QString title, QString message, QObject *targetModule, const char *onPasswordReceived, const char *onCancelled, const char *onDeniedAccess, const char *onError);

    void pingServer();
    void getAllMotionDevices();
    void requestServerInfo();

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
    static QString settingsFile;
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
    void onResponseReceived(Kijang::KijangProtocol response); // Can be sync or async
    void initClientID();
    void sendSystemInfoRequests();

    KijangMainModule moduleFFFE;
    KijangServerControlModule moduleFFFF;
    QMap<quint64, qint64> pingRecords;

signals:
    void responseReceived(quint64 requestID, Kijang::KijangProtocol response);
    void connectedChanged();
    void addressChanged();
    void portChanged();
    void attemptingConnectionChanged();
    void buttonStringChanged();

public slots:
    void moduleSendRequest(Kijang::KijangProtocol request);
    void moduleSetClientID(quint32 id);
    void modulePongReceived(quint64 requestID);
    void moduleServerName(QString name);
    void moduleServerVersion(QString version);
    void moduleServerRequestDisconnect();
    void moduleSystemInfoRequiresAuth(quint16 authType, QByteArray authDetails);
    void moduleSystemInfoAuthSuccess();
    void moduleSystemInfoAuthError();

private slots:
    void clientConnected();
    void clientDisconnected();
    void clientError(QAbstractSocket::SocketError socketError);
    void clientStateChanged(QAbstractSocket::SocketState socketState);
    void clientReadyRead();

};

#endif // KIJANGCLIENT_H
