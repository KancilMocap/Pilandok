#include "kijangwriteclient.h"

Q_DECLARE_LOGGING_CATEGORY(writeClient);
Q_LOGGING_CATEGORY(writeClient,"wclient");

KijangWriteClient::KijangWriteClient(QObject *parent) : KijangClient(parent)
{

}

void KijangWriteClient::setClientID(quint32 clientID)
{
    m_clientID = clientID;
}

void KijangWriteClient::sendRequest(KijangProtocol request) {
    if (m_connected) {
        socket.write(request.toByteArray());
        socket.waitForBytesWritten();
    } else {
        backlogRequestList.append(request);
    }
}

void KijangWriteClient::onConnected()
{
    // Write client ID to server
    QByteArray response;
    QDataStream stream(&response, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << m_clientID;
    socket.write(response);
    socket.waitForBytesWritten();

    for (KijangProtocol pendingPacket : backlogRequestList) {
        socket.write(pendingPacket.toByteArray());
        socket.waitForBytesWritten();
    }
}
