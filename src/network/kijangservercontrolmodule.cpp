#include "kijangservercontrolmodule.h"

Q_DECLARE_LOGGING_CATEGORY(serverControl);
Q_LOGGING_CATEGORY(serverControl,"clientModule-FFFF");

KijangServerControlModule::KijangServerControlModule(QObject *parent) : KijangClientModule(parent)
{
    m_module = 65535; // FFFF - Control module (server)
}

void KijangServerControlModule::handleResponse(KijangProtocol response)
{
    QString responseString = QString::fromUtf8(response.data(), response.data().size());
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
        emit pongReceived(response.requestID());
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
        qDebug(serverControl) << "Server name (0004) received as " << responseString;
        emit serverName(responseString);
        break;
    }
    case 5: { // 0005
        qDebug(serverControl) << "Server version (0005) received as " << responseString;
        emit serverVersion(responseString);
        break;
    }
    case 14: { // 000E
        qDebug(serverControl) << "Server request to disconnect (000E)";
        emit serverRequestDisconnect();
        break;
    }

    case 16: { // 0010
        QByteArray responseData = response.data();
        if (responseData.size() == 0) {
            emit systemInfoAuthSuccess();
            break;
        } else if (responseData.size() == 1) {
            qWarning(serverControl) << "Invalid data size provided for password auth, unable to proceed";
            break;
        }

        QByteArray authTypeArray = responseData.left(2);
        quint16 authType = qFromBigEndian<quint16>(authTypeArray);
        responseData.remove(0, 2);
        emit systemInfoRequiresAuth(authType, authTypeArray);
    }

    case 17: { // 0011
        if (response.data().size() != 1) {
            qWarning(serverControl) << "No authentication results returned for verifying blocks 0010-002F";
            break;
        }

        if (response.data().at(0) == '\0') emit systemInfoAuthSuccess();
        else emit systemInfoAuthError();
    }

    case 18: { // 0012
        qInfo(serverControl) << "Motherboard information: " << responseString;
        break;
    }

    case 19: { // 0013
        qInfo(serverControl) << "CPU information: " << responseString;
        break;
    }

    case 20: { // 0014
        qInfo(serverControl) << "GPU information: " << responseString;
        break;
    }

    case 21: { // 0015
        qInfo(serverControl) << "Hard disk information: " << responseString;
        break;
    }

    case 22: { // 0016
        qInfo(serverControl) << "RAM information: " << responseString;
        break;
    }

    case 23: { // 0017
        qInfo(serverControl) << "Peripheral devices information: " << responseString;
        break;
    }

    case 32: { // 0020
        qInfo(serverControl) << "Kernal name: " << responseString;
        break;
    }

    case 33: { // 0021
        qInfo(serverControl) << "Kernel version: " << responseString;
        break;
    }

    case 34: { // 0022
        qInfo(serverControl) << "OS type: " << responseString;
        break;
    }

    case 35: { // 0023
        qInfo(serverControl) << "OS version: " << responseString;
        break;
    }
    default: {
        qWarning(serverControl) << "Invalid response code " << response.code() << " received, unable to handle";
        break;
    }
    }
}

void KijangServerControlModule::systemInfoAuthReceived(quint32 clientID, QByteArray response)
{
    KijangProtocol request(clientID, 32767, 17); // 7FFF, 0011
    emit sendRequest(request);
}
