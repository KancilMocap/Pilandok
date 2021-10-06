#include "kijangservercontrolmodule.h"

Q_DECLARE_LOGGING_CATEGORY(serverControl);
Q_LOGGING_CATEGORY(serverControl,"clientModule-FFFE");

KijangServerControlModule::KijangServerControlModule(QObject *parent) : KijangClientModule(parent)
{
    m_module = 65534; // FFFE - Control module (server)
}

void KijangServerControlModule::handleResponse(KijangProtocol response)
{
    switch (response.code()) {
    case 0: { // 0000
        qInfo(serverControl) << "Generic response (0000) code received, data is " << response.data().toHex();
        break;
    }
    case 1: { // 0001
        qDebug(serverControl) << "Response from server (0001) code received, client ID is " << response.clientID();
        emit setClientID(response.clientID());
        break;
    }
    case 2: { // 0002
        qDebug(serverControl) << "Pong response (0002) received from server";
        emit pongReceived();
        break;
    }
    case 3: { // 0003
        // Ping is not forwarded
        qDebug(serverControl) << "Ping request (0003) received from server, sending response";
        response.setModule(32767); // 7FFF
        emit sendRequest(response);
        break;
    }
    case 4: { // 0004
        QString sServerName = QString::fromUtf8(response.data(), response.data().size());
        qDebug(serverControl) << "Server name (0004) received as " << sServerName;
        emit serverName(sServerName);
        break;
    }
    case 5: { // 0005
        QString sServerVersion = QString::fromUtf8(response.data(), response.data().size());
        qDebug(serverControl) << "Server version (0005) received as " << sServerVersion;
        emit serverVersion(sServerVersion);
        break;
    }
    case 14: { // 000E
        qDebug(serverControl) << "Server request to disconnect (000E)";
        emit serverRequestDisconnect();
        break;
    }
    default: {
        qWarning(serverControl) << "Invalid response code " << response.code() << " received, unable to handle";
        break;
    }
    }
}
