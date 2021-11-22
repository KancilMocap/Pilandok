#include "kijangservercontrolmodule.h"

Q_DECLARE_LOGGING_CATEGORY(serverControl);
Q_LOGGING_CATEGORY(serverControl,"clientModule-FFFF");

KijangServerControlModule::KijangServerControlModule(QObject *parent) : QObject(parent)
{

}

void KijangServerControlModule::handleResponse(KijangProtocol res)
{
    QString responseString = QString::fromUtf8(res.data(), res.data().size());
    switch (res.code()) {
    case 0: { // 0000
        qInfo(serverControl) << "Generic response (0000) code received, data is " << res.data().toHex();
        break;
    }
    case 1: { // 0001
        qDebug(serverControl) << "Response from server (0001) code received, client ID is " << res.clientID();
        m_clientID = res.clientID();
        emit clientIDReceived(m_clientID);
        pingServer();
        break;
    }
    case 2: { // 0002
        qDebug(serverControl) << "Pong response (0002) received from server";
        pongReceived(res.requestID());
        break;
    }
    case 3: { // 0003
        qDebug(serverControl) << "Ping request (0003) received from server, sending response";
        res.setModule(32767); // 7FFF
        emit sendRequest(res);
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
        QByteArray responseData = res.data();
        if (responseData.size() == 0) {
            sendSystemInfoRequests();
            break;
        } else if (responseData.size() == 1) {
            qWarning(serverControl) << "Invalid data size provided for password auth, unable to proceed";
            break;
        }

        QByteArray authTypeArray = responseData.left(2);
        quint16 authType = qFromBigEndian<quint16>(authTypeArray);
        responseData.remove(0, 2);
        emit requestAuth(65535, 32767, 17, authType, responseData, "server hardware and software information");
    }
    case 17: { // 0011
        if (res.data().size() != 1) {
            qWarning(serverControl) << "No authentication results returned for verifying blocks 0010-002F";
            break;
        }

        if (res.data().at(0) == '\0') sendSystemInfoRequests();
        else qWarning(serverControl) << "Authentication failed for getting system info, will not continue";
    }
    default: {
        handleSystemInfoRequest(true, 0, res);
        break;
    }
    }
}

quint16 KijangServerControlModule::module()
{
    return 65535; // FFFF
}

QString KijangServerControlModule::description()
{
    return "server control module";
}

void KijangServerControlModule::handleLocalResponse(quint16 src, KijangProtocol req)
{
    handleSystemInfoRequest(false, src, req);
}

bool KijangServerControlModule::canHandleCode(quint16 code)
{
    // Sent by local module, so local responses is only 0012-0017, 0020-0023
    return (code >= 18 && code <= 23) || (code >= 32 && code <= 35);
}

void KijangServerControlModule::modulePresent(quint16 module, bool present)
{
}

void KijangServerControlModule::codePresent(quint16 module, quint16 code, bool present)
{
}

void KijangServerControlModule::onStart()
{

}

void KijangServerControlModule::onStop()
{

}

void KijangServerControlModule::pingServer()
{
    KijangProtocol request(m_clientID, 32767, 2); // 7FFF, 0002
    emit sendRequest(request);
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    pingRecordsMutex.lock();
    pingRecords.insert(request.requestID(), startTime);
    pingRecordsMutex.unlock();
}

void KijangServerControlModule::setClientID(quint32 client)
{
    // Ignored as only set by this module
}

void KijangServerControlModule::pongReceived(quint64 requestID)
{
    qint64 endTime = QDateTime::currentMSecsSinceEpoch();
    pingRecordsMutex.lock();
    if (!pingRecords.contains(requestID) || !pingRecords.value(requestID)) {
        qWarning(serverControl) << "Pong received but no corresponding ping found";
        pingRecordsMutex.unlock();
        return;
    }
    qint64 startTime = pingRecords.value(requestID);
    pingRecords.remove(requestID);
    pingRecordsMutex.unlock();
    qint64 ping = (endTime - startTime) / 2;
    qInfo(serverControl) << "Current ping: " << ping << "ms";
}

void KijangServerControlModule::requestServerInfo()
{
    // Server name
    KijangProtocol request1(m_clientID, 32767, 4); // 7FFF, 0004
    emit sendRequest(request1);
    // Server version
    KijangProtocol request2(m_clientID, 32767, 5); // 7FFF, 0005
    emit sendRequest(request2);
}

void KijangServerControlModule::handleSystemInfoRequest(bool isRemote, quint16 src, KijangProtocol req)
{
    // Only for 18-27 b10 (0012 - 002F)
    QString responseString = QString::fromUtf8(req.data(), req.data().size());
    switch (req.code()) {
    case 18: { // 0012
        if (isRemote) {
            qInfo(serverControl) << "Motherboard information: " << responseString;
            motherboardInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(motherboardInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 19: { // 0013
        if (isRemote) {
            qInfo(serverControl) << "CPU information: " << responseString;
            cpuInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(cpuInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 20: { // 0014
        if (isRemote) {
            qInfo(serverControl) << "GPU information: " << responseString;
            gpuInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(gpuInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 21: { // 0015
        if (isRemote) {
            qInfo(serverControl) << "Hard disk information: " << responseString;
            hardDiskInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(hardDiskInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 22: { // 0016
        if (isRemote) {
            qInfo(serverControl) << "RAM information: " << responseString;
            ramInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(ramInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 23: { // 0017
        if (isRemote) {
            qInfo(serverControl) << "Peripheral devices information: " << responseString;
            peripheralInfo = responseString;
        } else {
            req.setModule(module());
            req.setData(peripheralInfo.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 32: { // 0020
        if (isRemote) {
            qInfo(serverControl) << "Kernal name: " << responseString;
            kernelName = responseString;
        } else {
            req.setModule(module());
            req.setData(kernelName.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 33: { // 0021
        if (isRemote) {
            qInfo(serverControl) << "Kernal version: " << responseString;
            kernelVersion = responseString;
        } else {
            req.setModule(module());
            req.setData(kernelVersion.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 34: { // 0022
        if (isRemote) {
            qInfo(serverControl) << "OS type: " << responseString;
            osType = responseString;
        } else {
            req.setModule(module());
            req.setData(osType.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }

    case 35: { // 0023
        if (isRemote) {
            qInfo(serverControl) << "OS version: " << responseString;
            osVersion = responseString;
        } else {
            req.setModule(module());
            req.setData(osVersion.toUtf8());
            emit sendLocalRequest(module(), src, req);
        }
        break;
    }
    default: {
        qWarning(serverControl) << "Invalid response code " << req.code() << " received, unable to handle";
        break;
    }
}
}

void KijangServerControlModule::sendSystemInfoRequests()
{
    // requestxx(m_clientID, 7FFF, yy)
    // xx is hex, yy is decimal
    KijangProtocol request12(m_clientID, 32767, 18);
    emit sendRequest(request12);
    KijangProtocol request13(m_clientID, 32767, 19);
    emit sendRequest(request13);
    KijangProtocol request14(m_clientID, 32767, 20);
    emit sendRequest(request14);
    KijangProtocol request15(m_clientID, 32767, 21);
    emit sendRequest(request15);
    KijangProtocol request16(m_clientID, 32767, 22);
    emit sendRequest(request16);
    KijangProtocol request17(m_clientID, 32767, 23);
    emit sendRequest(request17);
    KijangProtocol request20(m_clientID, 32767, 32);
    emit sendRequest(request20);
    KijangProtocol request21(m_clientID, 32767, 33);
    emit sendRequest(request21);
    KijangProtocol request22(m_clientID, 32767, 34);
    emit sendRequest(request22);
    KijangProtocol request23(m_clientID, 32767, 35);
    emit sendRequest(request23);
}

void KijangServerControlModule::setServerName(QString name)
{
    // Ignored as only sent by this module
}

void KijangServerControlModule::setServerVersion(QString version)
{
    // Ignored as only sent by this module
    // Server version needed in auth to access hardware info
    KijangProtocol request3(m_clientID, 32767, 16); // 7FFF, 0016
    emit sendRequest(request3);
}

void KijangServerControlModule::authFail(AuthFailureType failureType, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails)
{
    qInfo(serverControl) << "Authentication failed to access hardware, would not attempt further to authenticates";
}
