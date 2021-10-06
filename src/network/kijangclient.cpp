#include "kijangclient.h"

Q_DECLARE_LOGGING_CATEGORY(client);
Q_LOGGING_CATEGORY(client,"client");

KijangClient::KijangClient(QObject *parent) : QObject(parent)
{
    setConnected(false);
    setAttemptingConnection(false);
    setButtonString("Connect");
    internalConnectionSet = false;
    waitingForDisconnectReceived = false;
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

        // FFFE handlers
        connect(&moduleFFFE,&KijangServerControlModule::sendRequest,this,&KijangClient::moduleSendRequest);
        connect(&moduleFFFE,&KijangServerControlModule::setClientID,this,&KijangClient::moduleSetClientID);
        connect(&moduleFFFE,&KijangServerControlModule::pongReceived,this,&KijangClient::modulePongReceived);
        connect(&moduleFFFE,&KijangServerControlModule::serverName,this,&KijangClient::moduleServerName);
        connect(&moduleFFFE,&KijangServerControlModule::serverVersion,this,&KijangClient::moduleServerVersion);
        connect(&moduleFFFE,&KijangServerControlModule::serverRequestDisconnect,this,&KijangClient::moduleServerRequestDisconnect);
        internalConnectionSet = true;
    }
    socket.connectToHost(m_address, m_port);
}

void KijangClient::disconnectFromServer()
{
    readyRead = false;
    setAttemptingConnection(false);
    setConnected(false);
    if (socket.isOpen()) {
        // Send disconnect notification
        KijangProtocol disconnectRequest(m_clientID);
        disconnectRequest.setModule(32767); // 7FFF
        disconnectRequest.setCode(16); // 000F
        asyncSendRequest(disconnectRequest);
        socket.disconnectFromHost();
    }
    m_connected = false;
    waitingForDisconnectReceived = false;
}

void KijangClient::asyncSendRequest(KijangProtocol request)
{
    socket.write(request.toByteArray());
    socket.waitForBytesWritten();
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
    m_port = newPort;
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

void KijangClient::onResponseReceived(KijangProtocol response)
{
    // Ignore if response is to wait
    if (response.module() == 65535) return; // FFFF - Async wait

    if (m_clientID != response.clientID() && m_clientID) {
        qDebug(client) << "Response received for different client " << response.clientID() << ", would not handle";
        return;
    }

    switch (response.module()) {
    case 65533: { // FFFD - Kijang module
        // TODO: Complete
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
    KijangProtocol initRequest;
    initRequest.setModule(32767); // 7FFF - Control module
    initRequest.setCode(1); // 0001
    asyncSendRequest(initRequest);
}

void KijangClient::moduleSendRequest(KijangProtocol request) {
    asyncSendRequest(request); // Forward request
}

void KijangClient::moduleSetClientID(quint32 id)
{
    qInfo(client) << "Client ID set as " << id;
    m_clientID = id;
}

void KijangClient::modulePongReceived()
{

}

void KijangClient::moduleServerName(QString name)
{
    m_serverName = name;
}

void KijangClient::moduleServerVersion(QString version)
{
    m_serverVersion = version;
}

void KijangClient::moduleServerRequestDisconnect()
{
    disconnectFromServer();
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
    qInfo(client) << "Client state changed" << socketState;
}

void KijangClient::clientReadyRead() {
    QByteArray currentData = socket.readAll();
    qDebug(client) << "Data received:" << currentData.toHex();
    KijangProtocol response(currentData);
    if (response.exceptionInfo() == KijangProtocol::ExceptionInfo::NONE) {
        onResponseReceived(response);
    } else {
        qWarning(client) << "Unable to parse data from client as";
    }
}
