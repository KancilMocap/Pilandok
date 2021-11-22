#include "kijangclient.h"

Q_DECLARE_LOGGING_CATEGORY(client);
Q_LOGGING_CATEGORY(client,"client");

KijangClient::KijangClient(QObject *parent) : QObject(parent)
{
    internalConnectionSet = false;
}

KijangClient::~KijangClient()
{

}

void KijangClient::setPort(quint16 newPort)
{
    m_port = newPort;
}

void KijangClient::clientConnected() {
    readyRead = true;
}

void KijangClient::clientDisconnected() {
    readyRead = false;
}

void KijangClient::clientError(QAbstractSocket::SocketError socketError) {
    qWarning(client) << "Client error" << socketError;
}

void KijangClient::clientStateChanged(QAbstractSocket::SocketState socketState) {
    qDebug(client) << "Client state changed" << socketState;
    if (socketState == QAbstractSocket::SocketState::UnconnectedState) {
        readyRead = false;
    }
}

void KijangClient::clientReadyRead() {
    QByteArray currentData = socket.readAll();
    KijangProtocol response(currentData);
    if (response.exceptionInfo() == KijangProtocol::ExceptionInfo::NONE) {
        onResponseReceived(response);
    } else {
        qWarning(client) << "Unable to parse data from client as";
    }
}
