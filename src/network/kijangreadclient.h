#ifndef KIJANGREADCLIENT_H
#define KIJANGREADCLIENT_H

#include "kijangclient.h"
#include "kijangprotocol.h"
#include <QTimer>

class KijangReadClient : public KijangClient
{
    Q_OBJECT
public:
    explicit KijangReadClient(QObject *parent = nullptr);

signals:
    void responseReceived(KijangProtocol res);
    void writeDataReceived(quint16 writePort, quint32 clientID);

private slots:
    void clientReadyRead();
    void readClientTimeoutReached();

private:
    bool writePortSet = false;

    // KijangClient interface
public:
    void onConnected() Q_DECL_OVERRIDE;
};

#endif // KIJANGREADCLIENT_H
