#ifndef KIJANGWRITECLIENT_H
#define KIJANGWRITECLIENT_H

#include "kijangclient.h"
#include <QLoggingCategory>

class KijangWriteClient : public KijangClient
{
    Q_OBJECT
public:
    explicit KijangWriteClient(QObject *parent = nullptr);
    void setClientID(quint32 clientID);

public slots:
    void sendRequest(KijangProtocol request);

    // KijangClient interface
public:
    void onConnected() Q_DECL_OVERRIDE;

private:
    quint32 m_clientID;
    QList<KijangProtocol> backlogRequestList;
};

#endif // KIJANGWRITECLIENT_H
