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
    void pongReceived();
    void serverName(QString name);
    void serverVersion(QString version);
    void serverRequestDisconnect();

public:
    void handleResponse(KijangProtocol response) Q_DECL_OVERRIDE;
};

#endif // KIJANGSERVERCONTROLMODULE_H
