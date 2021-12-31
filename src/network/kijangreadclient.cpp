#include "kijangreadclient.h"

Q_DECLARE_LOGGING_CATEGORY(readClient);
Q_LOGGING_CATEGORY(readClient,"rclient");

KijangReadClient::KijangReadClient(QObject *parent) : KijangClient(parent)
{
    connect(&socket, &QAbstractSocket::readyRead, this, &KijangReadClient::clientReadyRead);
}

void KijangReadClient::clientReadyRead()
{
    QByteArray currentData = socket.readAll();
    if (writePortSet) {
        KijangProtocol response(currentData);
        if (response.exceptionInfo() == KijangProtocol::ExceptionInfo::NONE) {
            emit responseReceived(response);
        } else {
            qWarning(readClient) << "Unable to parse data from server, raw data is" << currentData.toHex();
        }
    } else {
        // Initial request must sent quint16 byte write port followed by quint32 client ID
        if (currentData.size() != 6) {
            qWarning(readClient) << "Invalid write port received from server, disconnecting";
            disconnectFromServer();
            return;
        }

        writePortSet = true;
        QByteArray writePortArray = currentData.left(2);
        currentData.remove(0, 2);
        emit writeDataReceived(qFromBigEndian<quint16>(writePortArray), qFromBigEndian<quint32>(currentData));
    }
}

void KijangReadClient::onConnected()
{
    // 5 seconds is hardcoded for now as specified in the connection procedures
    QTimer::singleShot(5000, this, &KijangReadClient::readClientTimeoutReached);
}

void KijangReadClient::readClientTimeoutReached() {
    // Checks if the write port has been received, and exits if not
    if (!writePortSet) {
        qWarning(readClient) << "Client information was not received within the specified interval, disconnecting";
        disconnectFromServer();
    }
}
