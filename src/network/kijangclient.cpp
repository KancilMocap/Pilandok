#include "kijangclient.h"

KijangClient::KijangClient(QObject *parent) : QObject(parent)
{

}

bool KijangClient::connect()
{
    m_connected = true;

    return true;
}

bool KijangClient::disconnect()
{
    m_connected = false;

    return true;
}

KijangProtocol KijangClient::syncSendRequest(KijangProtocol request)
{
    KijangProtocol response;
    return response;
}

void KijangClient::asyncSendRequest(KijangProtocol request)
{

}

const QString &KijangClient::address() const
{
    return m_address;
}

void KijangClient::setAddress(const QString &newAddress)
{
    m_address = newAddress;
    emit addressChanged();
}

quint16 KijangClient::port() const
{
    return m_port;
}

void KijangClient::setPort(quint16 newPort)
{
    m_port = newPort;
}

bool KijangClient::connected() const
{
    return m_connected;
}

void KijangClient::onResponseReceived(KijangProtocol response)
{

}
