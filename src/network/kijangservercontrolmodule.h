#ifndef KIJANGSERVERCONTROLMODULE_H
#define KIJANGSERVERCONTROLMODULE_H

#include "kijangclientmodule.h"
#include <QObject>
#include <QDebug>
#include <QLoggingCategory>

class KijangServerControlModule : public KijangClientModule
{
    Q_OBJECT
public:
    explicit KijangServerControlModule(QObject *parent = nullptr);

    // KijangClientModule interface

signals:
    void setClientID(quint32 id);
    void pongReceived(quint64 requestID);
    void serverName(QString name);
    void serverVersion(QString version);
    void serverRequestDisconnect();
    void systemInfoRequiresAuth(quint16 authType, QByteArray authDetails);
    void systemInfoAuthSuccess();
    void systemInfoAuthError();

public:
    void handleResponse(Kijang::KijangProtocol response) Q_DECL_OVERRIDE;
    void systemInfoAuthReceived(quint32 clientID, QByteArray response); // Note: Argument name MUST be "response" due to QMetaObject call from KijangClient::requestPassword
};

#endif // KIJANGSERVERCONTROLMODULE_H
