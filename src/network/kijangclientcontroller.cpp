#include "kijangclientcontroller.h"

Q_DECLARE_LOGGING_CATEGORY(clientController);
Q_LOGGING_CATEGORY(clientController,"clientcontrol");

QString KijangClientController::settingsFile = QDir::currentPath() + QDir::separator() + "settings.ini";

KijangClientController::KijangClientController(QObject *parent) : QObject(parent)
{
    setShowConnected(false);

    // Load address and port
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("client");
    setAddress(settings.value("address", "localhost").toString());
    setReadPort(settings.value("read_port", 4318).toUInt());
    settings.endGroup();

    // Connect read and write clients
    connect(&m_readClient, &KijangClient::connectedChanged, this, &KijangClientController::readConnectedChanged);
    connect(&m_readClient, &KijangClient::attemptingConnectionChanged, this, &KijangClientController::attemptingConnectionChanged);
    connect(&m_writeClient, &KijangClient::connectedChanged, this, &KijangClientController::writeConnectedChanged);
    connect(&m_writeClient, &KijangClient::attemptingConnectionChanged, this, &KijangClientController::attemptingConnectionChanged);
    connect(&m_readClient, &KijangReadClient::responseReceived, this, &KijangClientController::responseReceived);
    connect(&m_readClient, &KijangReadClient::writeDataReceived, this, &KijangClientController::writeDataReceived);
}

KijangClientController::~KijangClientController()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("client");
    settings.setValue("address", m_address);
    settings.setValue("read_port", m_readPort);
    settings.endGroup();
    settings.sync();
}

void KijangClientController::toggleConnection()
{
    if (m_readClient.connected() || m_readClient.attemptingConnection() || m_writeClient.connected() || m_writeClient.attemptingConnection()) {
        m_readClient.disconnectFromServer();
        m_writeClient.disconnectFromServer();
    } else {
        m_readClient.connectToServer();
    }
}

const QString &KijangClientController::address() const
{
    return m_address;
}

void KijangClientController::setAddress(const QString &newAddress)
{
    if (m_address != newAddress) {
        m_address = newAddress;
        emit addressChanged();
    }
}

quint16 KijangClientController::readPort() const
{
    return m_readPort;
}

void KijangClientController::setReadPort(quint16 newPort)
{
    if (m_readPort != newPort) {
        m_readPort = newPort;
        m_readClient.setPort(newPort);
        emit readPortChanged();
    }
}

const QString &KijangClientController::serverNameString() const
{
    return m_serverName;
}

const QString &KijangClientController::serverVersionString() const
{
    return m_serverVersion;
}

bool KijangClientController::showConnected() const
{
    return m_showConnected;
}

void KijangClientController::setShowConnected(bool newShowConnected)
{
    if (m_showConnected == newShowConnected)
        return;
    m_showConnected = newShowConnected;
    emit showConnectedChanged();
}

bool KijangClientController::lockConnectButton() const
{
    return m_lockConnectButton;
}

void KijangClientController::setLockConnectButton(bool newLockConnectButton)
{
    if (m_lockConnectButton == newLockConnectButton)
        return;
    m_lockConnectButton = newLockConnectButton;
    emit lockConnectButtonChanged();
}

void KijangClientController::addModule(KijangClientModule *module)
{
    if (!module) {
        qWarning(clientController) << "Attempt to add non-existent module object";
        return;
    }
    quint16 moduleID = module->module();
    if (moduleID == 65535 || moduleID == 65534) {
        qWarning(clientController) << "Protected modules could not be added";
        return;
    }
    if (moduleMap.contains(moduleID)) {
        qWarning(clientController) << "Attempt to add module" << moduleID << "but another module with the same ID already exists";
        return;
    }
    connect(dynamic_cast<QObject*>(module), SIGNAL(sendRequest(KijangProtocol)), this, SLOT(sendRequest(KijangProtocol)));
    connect(dynamic_cast<QObject*>(module), SIGNAL(sendLocalRequest(quint16,quint16,KijangProtocol)), this, SLOT(sendLocalRequest(quint16,quint16,KijangProtocol)));
    connect(dynamic_cast<QObject*>(module), SIGNAL(checkModulePresent(quint16,quint16)), this, SLOT(checkModulePresent(quint16,quint16)));
    connect(dynamic_cast<QObject*>(module), SIGNAL(checkCodePresent(quint16,quint16,quint16)), this, SLOT(checkCodePresent(quint16,quint16,quint16)));
    connect(dynamic_cast<QObject*>(module), SIGNAL(requestAuth(quint16,quint16,quint16,quint16,QByteArray,QString)), this, SLOT(requestAuth(quint16,quint16,quint16,quint16,QByteArray,QString)));
    moduleMap.insert(moduleID, module);
    qInfo(clientController) << "Module" << moduleID << "(" << module->description() << ")added";
}

void KijangClientController::removeModule(quint16 moduleID)
{
    if (moduleID == 65535 || moduleID == 65534) {
        qWarning(clientController) << "Protected modules could not be removed";
        return;
    }
    if (!moduleMap.contains(moduleID)) {
        qWarning(clientController) << "Attempt to remove module" << moduleID << "but no corresponding module exists";
        return;
    }
    KijangClientModule *module = moduleMap.value(moduleID);
    disconnect(dynamic_cast<QObject*>(module), SIGNAL(sendRequest(KijangProtocol)), this, SLOT(sendRequest(KijangProtocol)));
    disconnect(dynamic_cast<QObject*>(module), SIGNAL(sendLocalRequest(quint16,quint16,KijangProtocol)), this, SLOT(sendLocalRequest(quint16,quint16,KijangProtocol)));
    disconnect(dynamic_cast<QObject*>(module), SIGNAL(checkModulePresent(quint16,quint16)), this, SLOT(checkModulePresent(quint16,quint16)));
    disconnect(dynamic_cast<QObject*>(module), SIGNAL(checkCodePresent(quint16,quint16,quint16)), this, SLOT(checkCodePresent(quint16,quint16,quint16)));
    disconnect(dynamic_cast<QObject*>(module), SIGNAL(requestAuth(quint16,quint16,quint16,quint16,QByteArray,QString)), this, SLOT(requestAuth(quint16,quint16,quint16,quint16,QByteArray,QString)));
    delete module;
    moduleMap.remove(moduleID);
    qInfo(clientController) << "Module" << moduleID << "removed";
}

bool KijangClientController::inputsLocked() const
{
    return m_inputsLocked;
}

void KijangClientController::setInputsLocked(bool newInputsLocked)
{
    if (m_inputsLocked == newInputsLocked)
        return;
    m_inputsLocked = newInputsLocked;
    emit inputsLockedChanged();
}

void KijangClientController::clientIDReceived(quint32 id)
{
    qInfo(clientController) << "Client ID set as " << id;
    m_clientID = id;
    for (KijangClientModule *module : moduleMap) {
        module->setClientID(id);
    }
    m_writeClient.setClientID(m_clientID);
    m_writeClient.connectToServer();
}

void KijangClientController::serverNameReceived(QString name)
{
    m_serverName = name;
    for (KijangClientModule *module : moduleMap) {
        module->setServerName(name);
    }
}

void KijangClientController::serverVersionReceived(QString version)
{
    m_serverVersion = version;
    for (KijangClientModule *module : moduleMap) {
        module->setServerVersion(version);
    }
}

void KijangClientController::serverRequestDisconnect()
{
    m_readClient.disconnectFromServer();
    m_writeClient.disconnectFromServer();
}

void KijangClientController::responseReceived(KijangProtocol res)
{
    quint16 module = res.module();
    if (!moduleMap.contains(module)) {
        qWarning(clientController) << "Module" << module << "returned from server but no module present is capable of handling it";
        return;
    }
    moduleMap.value(module)->handleResponse(res);
}

void KijangClientController::sendRequest(KijangProtocol request)
{
    if (!m_readClient.connected()) {
        qWarning(clientController) << "Request unsent as client is not connected to server";
        return;
    }
    m_writeClient.sendRequest(request);
}

void KijangClientController::sendLocalRequest(quint16 src, quint16 target, KijangProtocol req)
{
    if (!moduleMap.contains(src)) {
        qWarning(clientController) << "Module" << src << "claims to sent requests but no such module exists";
        return;
    }
    if (!moduleMap.contains(target)) {
        moduleMap.value(src)->modulePresent(target, false);
        return;
    }
    moduleMap.value(target)->handleLocalResponse(src, req);
}

void KijangClientController::checkModulePresent(quint16 src, quint16 module)
{
    if (!moduleMap.contains(src)) {
        qWarning(clientController) << "Module" << src << "claims to sent requests but no such module exists";
        return;
    }
    moduleMap.value(module)->modulePresent(module, moduleMap.contains(module));
}

void KijangClientController::checkCodePresent(quint16 src, quint16 module, quint16 code)
{
    if (!moduleMap.contains(src)) {
        qWarning(clientController) << "Module" << src << "claims to sent requests but no such module exists";
        return;
    }
    if (!moduleMap.contains(module)) {
        moduleMap.value(src)->codePresent(module, code, false);
        return;
    }
    moduleMap.value(src)->codePresent(module, code, moduleMap.value(module)->canHandleCode(code));
}

void KijangClientController::requestAuth(quint16 src, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails, QString authReason)
{
    if (!moduleMap.contains(src)) {
        qWarning(clientController) << "Module" << src << "claims to require authentication but no such module exists";
        return;
    }
    KijangClientModule *currentModule = moduleMap.value(src);
    if (authMethod == 65535) {
        currentModule->authFail(KijangClientModule::ACCESS_BLOCKED, module, code, authMethod, authDetails);
        return;
    }

    bool ok;
    QWidget passwordPopup;
    QString message = QString("Module %1 (code %2) requires password authentication for %3 (code %4)")
            .arg(currentModule->description(), QString::number(src, 16), authReason, QString::number(code, 16));
    QString password = QInputDialog::getText(&passwordPopup, "Password Required", message, QLineEdit::Password, QString(), &ok);
    if (!ok || password.length() == 0) {
        currentModule->authFail(KijangClientModule::CANCELLED, module, code, authMethod, authDetails);
        return;
    }

    QByteArray utf8Array = password.toUtf8();
    KijangProtocol res(m_clientID, module, code);
    switch (authMethod) {
    case 1: {
        res.setData(utf8Array);
        sendRequest(res);
        break;
    }
    case 2: {
        // Check if iteration count is provided
        if (authDetails.size() != 4) {
            // Invalid iteration count
            currentModule->authFail(KijangClientModule::INVALID_PARAMETERS, module, code, authMethod, authDetails);
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
        // Send response
        res.setData(response);
        sendRequest(response);
        break;
    }
    case 3: {
        if (m_serverVersion.size() == 0) {
            qWarning(clientController) << "Server version not received, unable to hash password";
            currentModule->authFail(KijangClientModule::NO_SERVER_VERSION, module, code, authMethod, authDetails);
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
        // Send response
        res.setData(response);
        sendRequest(response);
        break;
    }
    default: {
        qWarning(clientController) << "Unable to process password due to incorrect auth type " << authMethod << " provided";
        currentModule->authFail(KijangClientModule::INCORRECT_AUTH_METHOD, module, code, authMethod, authDetails);
        break;
    }
    }
}

void KijangClientController::readConnectedChanged()
{
    if (m_readClient.connected()) {
        setInputsLocked(true);
    } else {
        m_writeClient.disconnectFromServer();
        setShowConnected(false);
        setInputsLocked(false);
    }
}

void KijangClientController::writeConnectedChanged()
{
    if (m_writeClient.connected()) {
        setInputsLocked(true);
        setShowConnected(true);
    } else {
        m_readClient.disconnectFromServer();
        setShowConnected(false);
        setInputsLocked(false);
    }
}

void KijangClientController::attemptingConnectionChanged()
{
    setInputsLocked(m_readClient.connected() || m_readClient.attemptingConnection() || m_writeClient.connected() || m_writeClient.attemptingConnection());
    setLockConnectButton(m_readClient.attemptingConnection() || m_writeClient.attemptingConnection());
}

void KijangClientController::writeDataReceived(quint16 writePort, quint32 clientID)
{
    setWritePort(writePort);
    clientIDReceived(clientID);
}

quint16 KijangClientController::writePort() const
{
    return m_writePort;
}

void KijangClientController::setWritePort(quint16 newWritePort)
{
    m_writePort = newWritePort;
    m_writeClient.setPort(newWritePort);
}
