#ifndef KIJANGWRITECLIENT_H
#define KIJANGWRITECLIENT_H

#include "kijangclient.h"

class KijangWriteClient : public KijangClient
{
    Q_OBJECT
public:
    explicit KijangWriteClient(QObject *parent = nullptr);

public slots:
    void sendRequest(KijangProtocol request);
};

#endif // KIJANGWRITECLIENT_H
