#ifndef KIJANGCLIENT_H
#define KIJANGCLIENT_H

#include "kijangprotocol.h"
#include <QObject>
#include <QDebug>

class KijangClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
public:
    explicit KijangClient(QObject *parent = nullptr);
    bool connect();
    bool disconnect();
    KijangProtocol syncSendRequest(KijangProtocol request);
    void asyncSendRequest(KijangProtocol request);

    const QString &address() const;
    void setAddress(const QString &newAddress);

    quint16 port() const;
    void setPort(quint16 newPort);

    bool connected() const;

private:
    bool m_connected;
    QString m_errorString;
    QString m_address;
    quint32 m_port;
    void onResponseReceived(KijangProtocol response);

signals:
    void responseReceived(KijangProtocol response);
    void addressChanged();
    void portChanged();

};

#endif // KIJANGCLIENT_H
