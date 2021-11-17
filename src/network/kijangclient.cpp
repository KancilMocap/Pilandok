#include "kijangclient.h"

Q_DECLARE_LOGGING_CATEGORY(client);
Q_LOGGING_CATEGORY(client,"client");

QString KijangClient::settingsFile = QDir::currentPath() + QDir::separator() + "settings.ini";

KijangClient::KijangClient(QObject *parent) : QObject(parent)
{
    setConnected(false);
    setAttemptingConnection(false);
    setButtonString("Connect");
    internalConnectionSet = false;
    waitingForDisconnectReceived = false;
    m_clientID = 0;

    // Load address and port
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("client");
    setAddress(settings.value("address", "localhost").toString());
    setPort(settings.value("port", 4318).toUInt());
    settings.endGroup();
}

KijangClient::~KijangClient()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("client");
    settings.setValue("address", m_address);
    settings.setValue("port", m_port);
    settings.endGroup();
    settings.sync();
}

void KijangClient::toggleConnection()
{
    if (!m_attemptingConnection) {
        if (m_connected) disconnectFromServer();
        else connectToServer();
    }
}

void KijangClient::connectToServer()
{
    readyRead = false;
    setAttemptingConnection(true);
    qInfo(client) << "Connecting to " << m_address << " port " << m_port;
    if (!internalConnectionSet) {
        connect(&socket,&QTcpSocket::connected,this,&KijangClient::clientConnected);
        connect(&socket,&QTcpSocket::disconnected,this,&KijangClient::clientDisconnected);
        connect(&socket,&QTcpSocket::stateChanged,this,&KijangClient::clientStateChanged);
        connect(&socket,&QTcpSocket::readyRead,this,&KijangClient::clientReadyRead);

        // FFFD handlers
        connect(&moduleFFFD,&KijangMainModule::sendRequest,this,&KijangClient::moduleSendRequest);

        // FFFE handlers
        connect(&moduleFFFE,&KijangServerControlModule::sendRequest,this,&KijangClient::moduleSendRequest);
        connect(&moduleFFFE,&KijangServerControlModule::setClientID,this,&KijangClient::moduleSetClientID);
        connect(&moduleFFFE,&KijangServerControlModule::pongReceived,this,&KijangClient::modulePongReceived);
        connect(&moduleFFFE,&KijangServerControlModule::serverName,this,&KijangClient::moduleServerName);
        connect(&moduleFFFE,&KijangServerControlModule::serverVersion,this,&KijangClient::moduleServerVersion);
        connect(&moduleFFFE,&KijangServerControlModule::serverRequestDisconnect,this,&KijangClient::moduleServerRequestDisconnect);
        connect(&moduleFFFE,&KijangServerControlModule::systemInfoRequiresAuth,this,&KijangClient::moduleSystemInfoRequiresAuth);
        connect(&moduleFFFE,&KijangServerControlModule::systemInfoAuthSuccess,this,&KijangClient::moduleSystemInfoAuthSuccess);
        connect(&moduleFFFE,&KijangServerControlModule::systemInfoAuthError,this,&KijangClient::moduleSystemInfoAuthError);
        internalConnectionSet = true;
    }
    socket.connectToHost(m_address, m_port);
}

void KijangClient::disconnectFromServer()
{
    readyRead = false;
    setAttemptingConnection(false);
    setConnected(false);
    waitingForDisconnectReceived = false;
    if (socket.isOpen()) {
        // Send disconnect notification
        Kijang::KijangProtocol disconnectRequest(m_clientID, 32767, 15); // 7FFF, 000F
        asyncSendRequest(disconnectRequest);
        socket.disconnectFromHost();
    }
}

void KijangClient::asyncSendRequest(Kijang::KijangProtocol request)
{
    socket.write(request.toByteArray());
    socket.waitForBytesWritten();
}

void KijangClient::requestPassword(quint16 authType, QByteArray authDetails, QString title, QString message, QObject *targetModule, const char* onPasswordReceived, const char* onCancelled, const char* onDeniedAccess, const char *onError)
{
    if (authType == 65535) {
        qInfo(client) << "System info block denied access";
        if (onDeniedAccess) QMetaObject::invokeMethod(targetModule, onDeniedAccess);
        return;
    }

    bool ok;
    QWidget passwordPopup;
    QString password = QInputDialog::getText(&passwordPopup, title, message, QLineEdit::Password, QString(), &ok);
    if (!ok || password.length() == 0) {
        if (onCancelled) QMetaObject::invokeMethod(targetModule, onCancelled);
        return;
    }

    QByteArray utf8Array = message.toUtf8();
    switch (authType) {
    case 1: {
        QMetaObject::invokeMethod(targetModule, onPasswordReceived, Q_ARG(QByteArray, utf8Array));
        break;
    }
    case 2: {
        // Check if iteration count is provided
        if (authDetails.size() != 4) {
            if (onError) QMetaObject::invokeMethod(targetModule, onError, Q_ARG(QString, "Invalid iteration count in PKCS12-PBKDF hashing"));
            break;
        }

        CryptoPP::byte derived[CryptoPP::BLAKE2b::DIGESTSIZE];
        CryptoPP::byte salt[32];

        QRandomGenerator rand;
        for (int i = 0; i < 4; i++) {
            CryptoPP::byte currentIntStd[8];
            quint64 currentInt = rand.generate64();
            std::copy(static_cast<const char*>(static_cast<const void*>(&currentInt)),
                      static_cast<const char*>(static_cast<const void*>(&currentInt)) + 8,
                      currentIntStd);
            for (int j = 0; j < 8; j++) {
                salt[i*8 + j] = currentIntStd[j];
            }
        }
        CryptoPP::PKCS12_PBKDF<CryptoPP::BLAKE2b> pbkdf;
        quint8 purpose = 201;
        quint32 iterations = qFromBigEndian<quint32>(authDetails);
        pbkdf.DeriveKey(derived, sizeof(derived), purpose, (unsigned char*)(utf8Array.data()), utf8Array.size(), salt, 32, iterations, 0.0f);

        char derivedChar[CryptoPP::BLAKE2b::DIGESTSIZE];
        for (unsigned int i = 0; i < CryptoPP::BLAKE2b::DIGESTSIZE; i++) {
            derivedChar[i] = (char)derived[i];
        }

        char saltChar[32];
        for (unsigned int i = 0; i < 32; i++) {
            saltChar[i] = (char)salt[i];
        }

        QByteArray derivedByteArray(derivedChar);
        QByteArray saltByteArray(saltChar);
        QByteArray response;
        QDataStream stream(&response, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        quint16 derivedByteArraySize = derivedByteArray.size();
        quint16 saltByteArraySize = saltByteArray.size();
        stream << derivedByteArraySize;
        response.append(derivedByteArray);
        stream << saltByteArraySize;
        response.append(saltByteArray);
        QMetaObject::invokeMethod(targetModule, onPasswordReceived, Q_ARG(quint32, m_clientID), Q_ARG(QString, response));
        break;
    }
    case 3: {
        if (m_serverVersion.size() == 0) {
            qWarning(client) << "Server version not received, unable to hash password";
            if (onError) QMetaObject::invokeMethod(targetModule, onError, Q_ARG(QString, "Server version not received, unable to hash password"));
            break;
        }

        CryptoPP::byte derived[CryptoPP::BLAKE2b::DIGESTSIZE];
        CryptoPP::byte salt[32];
        QByteArray infoArray = m_serverVersion.toUtf8();

        QRandomGenerator rand;
        for (int i = 0; i < 4; i++) {
            CryptoPP::byte currentIntStd[8];
            quint64 currentInt = rand.generate64();
            std::copy(static_cast<const char*>(static_cast<const void*>(&currentInt)),
                      static_cast<const char*>(static_cast<const void*>(&currentInt)) + 8,
                      currentIntStd);
            for (int j = 0; j < 8; j++) {
                salt[i*8 + j] = currentIntStd[j];
            }
        }
        CryptoPP::HKDF<CryptoPP::BLAKE2b> hkdf;
        hkdf.DeriveKey(derived, sizeof(derived), (unsigned char*)(utf8Array.data()), utf8Array.size(), salt, 32, (unsigned char*)(infoArray.data()), infoArray.size());

        char derivedChar[CryptoPP::BLAKE2b::DIGESTSIZE];
        for (unsigned int i = 0; i < CryptoPP::BLAKE2b::DIGESTSIZE; i++) {
            derivedChar[i] = (char)derived[i];
        }

        char saltChar[32];
        for (unsigned int i = 0; i < 32; i++) {
            saltChar[i] = (char)salt[i];
        }

        QByteArray derivedByteArray(derivedChar);
        QByteArray saltByteArray(saltChar);
        QByteArray response;
        QDataStream stream(&response, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        quint16 derivedByteArraySize = derivedByteArray.size();
        quint16 saltByteArraySize = saltByteArray.size();
        stream << derivedByteArraySize;
        response.append(derivedByteArray);
        stream << saltByteArraySize;
        response.append(saltByteArray);
        QMetaObject::invokeMethod(targetModule, onPasswordReceived, Q_ARG(quint32, m_clientID), Q_ARG(QString, response));
        break;
    }
    default: {
        qWarning(client) << "Unable to process password due to incorrect auth type " << authType << " provided";
        if (onError) QMetaObject::invokeMethod(targetModule, onError);
        break;
    }
    }
}

void KijangClient::pingServer()
{
    Kijang::KijangProtocol request(m_clientID, 32767, 2); // 7FFF, 0002
    asyncSendRequest(request);
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    pingRecords.insert(request.requestID(), startTime);
}

void KijangClient::getAllMotionDevices()
{
    Kijang::KijangProtocol request(m_clientID, 32766, 5); // 7FFE, 0005
    asyncSendRequest(request);
}

void KijangClient::requestServerInfo()
{
    // Server name
    Kijang::KijangProtocol request1(m_clientID, 32767, 4); // 7FFF, 0004
    asyncSendRequest(request1);
    // Server version
    Kijang::KijangProtocol request2(m_clientID, 32767, 5); // 7FFF, 0005
    asyncSendRequest(request2);
}

const QString &KijangClient::address() const
{
    return m_address;
}

void KijangClient::setAddress(const QString &newAddress)
{
    if (m_address != newAddress) {
        m_address = newAddress;
        emit addressChanged();
    }
}

quint16 KijangClient::port() const
{
    return m_port;
}

void KijangClient::setPort(quint16 newPort)
{
    if (m_port != newPort) {
        m_port = newPort;
        emit portChanged();
    }
}

bool KijangClient::connected() const
{
    return m_connected;
}

void KijangClient::setConnected(bool newConnected)
{
    if (m_connected == newConnected)
        return;

    if (newConnected) setButtonString("Disconnect");
    else setButtonString("Connect");
    m_connected = newConnected;
    emit connectedChanged();
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
    emit attemptingConnectionChanged();
}

const QString &KijangClient::buttonString() const
{
    return m_buttonString;
}

void KijangClient::setButtonString(const QString &newButtonString)
{
    if (m_buttonString == newButtonString)
        return;
    m_buttonString = newButtonString;
    emit buttonStringChanged();
}

const QString &KijangClient::serverName() const
{
    return m_serverName;
}

const QString &KijangClient::serverVersion() const
{
    return m_serverVersion;
}

void KijangClient::onResponseReceived(Kijang::KijangProtocol response)
{
    // Ignore if response is to wait
    if (response.module() == 65535) return; // FFFF - Async wait

    if (m_clientID != response.clientID() && m_clientID) {
        qDebug(client) << "Response received for different client " << response.clientID() << ", would not handle";
        return;
    }

    switch (response.module()) {
    case 65533: { // FFFD - Kijang module
        moduleFFFD.handleResponse(response);
    }
    case 65534: { // FFFE - Server control module
        moduleFFFE.handleResponse(response);
    }
    default: {
        emit responseReceived(response.requestID(), response);
    }
    }
}

void KijangClient::initClientID()
{
    // Create initial request
    Kijang::KijangProtocol initRequest;
    initRequest.setModule(32767); // 7FFF - Control module
    initRequest.setCode(1); // 0001
    asyncSendRequest(initRequest);
}

void KijangClient::sendSystemInfoRequests()
{
    // requestxx(m_clientID, 7FFF, yy)
    // xx is hex, yy is decimal
    Kijang::KijangProtocol request12(m_clientID, 32767, 18);
    asyncSendRequest(request12);
    Kijang::KijangProtocol request13(m_clientID, 32767, 19);
    asyncSendRequest(request13);
    Kijang::KijangProtocol request14(m_clientID, 32767, 20);
    asyncSendRequest(request14);
    Kijang::KijangProtocol request15(m_clientID, 32767, 21);
    asyncSendRequest(request15);
    Kijang::KijangProtocol request16(m_clientID, 32767, 22);
    asyncSendRequest(request16);
    Kijang::KijangProtocol request17(m_clientID, 32767, 23);
    asyncSendRequest(request17);
    Kijang::KijangProtocol request20(m_clientID, 32767, 32);
    asyncSendRequest(request20);
    Kijang::KijangProtocol request21(m_clientID, 32767, 33);
    asyncSendRequest(request21);
    Kijang::KijangProtocol request22(m_clientID, 32767, 34);
    asyncSendRequest(request22);
    Kijang::KijangProtocol request23(m_clientID, 32767, 35);
    asyncSendRequest(request23);
}

void KijangClient::moduleSendRequest(Kijang::KijangProtocol request) {
    asyncSendRequest(request); // Forward request
}

void KijangClient::moduleSetClientID(quint32 id)
{
    qInfo(client) << "Client ID set as " << id;
    m_clientID = id;
    pingServer();
    getAllMotionDevices();
    requestServerInfo();
}

void KijangClient::modulePongReceived(quint64 requestID)
{
    if (!pingRecords.contains(requestID) || !pingRecords.value(requestID)) qWarning(client) << "Pong received but no corresponding ping found";
    qint64 endTime = QDateTime::currentMSecsSinceEpoch();
    qint64 startTime = pingRecords.value(requestID);
    pingRecords.remove(requestID);
    qint64 ping = (endTime - startTime) / 2;
    qInfo(client) << "Current ping: " << ping << "ms";
}

void KijangClient::moduleServerName(QString name)
{
    m_serverName = name;
}

void KijangClient::moduleServerVersion(QString version)
{
    m_serverVersion = version;
    // Check if system info is blocked
    Kijang::KijangProtocol request(m_clientID, 32767, 16); // 7FFF, 0010
    asyncSendRequest(request);
}

void KijangClient::moduleServerRequestDisconnect()
{
    disconnectFromServer();
}

void KijangClient::moduleSystemInfoRequiresAuth(quint16 authType, QByteArray authDetails)
{
    requestPassword(authType, authDetails, "System Info Authentication", "A password is needed to access the system info of the device.", &moduleFFFE, SLOT(moduleFFFE.systemInfoAuthReceived), nullptr, nullptr, nullptr);
}

void KijangClient::moduleSystemInfoAuthSuccess()
{
    qInfo(client) << "Authentication for system info successful, sending requests for device information";
    sendSystemInfoRequests();
}

void KijangClient::moduleSystemInfoAuthError()
{
    qWarning(client) << "Authentication failed for getting system info, will not continue";
}

void KijangClient::clientConnected() {
    setAttemptingConnection(false);
    setConnected(true);
    readyRead = true;
    QThread thread;
    initClientID();
}

void KijangClient::clientDisconnected() {
    setAttemptingConnection(false);
    setConnected(false);
    readyRead = false;
}

void KijangClient::clientError(QAbstractSocket::SocketError socketError) {
    qWarning(client) << "Client error" << socketError;
}

void KijangClient::clientStateChanged(QAbstractSocket::SocketState socketState) {
    qDebug(client) << "Client state changed" << socketState;
    if (socketState == QAbstractSocket::SocketState::UnconnectedState) {
        readyRead = false;
        setAttemptingConnection(false);
        setConnected(false);
        waitingForDisconnectReceived = false;
    }
}

void KijangClient::clientReadyRead() {
    QByteArray currentData = socket.readAll();
    Kijang::KijangProtocol response(currentData);
    if (response.exceptionInfo() == Kijang::KijangProtocol::ExceptionInfo::NONE) {
        onResponseReceived(response);
    } else {
        qWarning(client) << "Unable to parse data from client as";
    }
}
