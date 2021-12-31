#include "kijangclient.h"

KijangClient::KijangClient(QObject *parent) : QObject(parent)
{
    internalConnectionSet = false;
    connect(&socket, &QAbstractSocket::connected, this, &KijangClient::clientConnected);
    connect(&socket, &QAbstractSocket::disconnected, this, &KijangClient::clientDisconnected);
    connect(&socket, &QAbstractSocket::stateChanged, this, &KijangClient::clientStateChanged);
}

KijangClient::~KijangClient()
{

}

void KijangClient::setPort(quint16 newPort)
{
    m_port = newPort;
}

void KijangClient::connectToServer()
{
    setAttemptingConnection(true);
    socket.connectToHost(m_address, m_port);
}

void KijangClient::disconnectFromServer()
{
    if (m_connected || m_attemptingConnection) socket.disconnectFromHost();
}

bool KijangClient::connected() const
{
    return m_connected;
}

void KijangClient::setConnected(bool newConnected)
{
    if (m_connected == newConnected)
        return;
    m_connected = newConnected;
    emit connectedChanged(newConnected);
}

bool KijangClient::attemptingConnection() const
{
    return m_attemptingConnection;
}

void KijangClient::setAttemptingConnection(bool newAttemptingConnection)
{
    if (m_attemptingConnection == newAttemptingConnection)
        return;
    m_attemptingConnection = newAttemptingConnection;
    emit attemptingConnectionChanged(newAttemptingConnection);
}

void KijangClient::clientConnected() {
    readyRead = true;
    setAttemptingConnection(false);
    setConnected(true);
    onConnected();
}

void KijangClient::clientDisconnected() {
    readyRead = false;
    setAttemptingConnection(false);
    setConnected(false);
}

void KijangClient::clientStateChanged(QAbstractSocket::SocketState socketState) {
    if (socketState == QAbstractSocket::SocketState::UnconnectedState) {
        readyRead = false;
    }
}
