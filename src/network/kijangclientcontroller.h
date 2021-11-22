#ifndef KIJANGCLIENTCONTROLLER_H
#define KIJANGCLIENTCONTROLLER_H

#include <QObject>
#include <QInputDialog>
#include <QSettings>
#include <cryptlib.h>
#include <blake2.h>
#include <pwdbased.h>
#include <hkdf.h>
#include "kijangclientmodule.h"
#include "kijangservercontrolmodule.h"
#include "kijangreadclient.h"
#include "kijangwriteclient.h"

class KijangClientController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(quint16 readPort READ readPort WRITE setReadPort NOTIFY readPortChanged)
    Q_PROPERTY(QString buttonString READ buttonString WRITE setButtonString NOTIFY buttonStringChanged)
    Q_PROPERTY(bool inputsLocked READ inputsLocked WRITE setInputsLocked NOTIFY inputsLockedChanged)
    Q_PROPERTY(bool showConnected READ showConnected WRITE setShowConnected NOTIFY showConnectedChanged)
    Q_PROPERTY(bool lockConnectButton READ lockConnectButton WRITE setLockConnectButton NOTIFY lockConnectButtonChanged)

public:
    explicit KijangClientController(QObject *parent = nullptr);
    ~KijangClientController();
    Q_INVOKABLE void toggleConnection();

    const QString &address() const;
    void setAddress(const QString &newAddress);
    quint16 readPort() const;
    void setReadPort(quint16 newPort);
    const QString &buttonString() const;
    void setButtonString(const QString &newButtonString);
    bool inputsLocked() const;
    void setInputsLocked(bool newInputsLocked);
    bool showConnected() const;
    void setShowConnected(bool newShowConnected);
    bool lockConnectButton() const;
    void setLockConnectButton(bool newLockConnectButton);

    void addModule(KijangClientModule *module);
    void removeModule(quint16 moduleID);

    const QString &serverNameString() const;
    const QString &serverVersionString() const;

public slots:
    void clientIDReceived(quint32 id);
    void serverNameReceived(QString name);
    void serverVersionReceived(QString version);
    void serverRequestDisconnect();

    // Client module slots
    void sendRequest(KijangProtocol request);
    void sendLocalRequest(quint16 src, quint16 target, KijangProtocol req);
    void checkModulePresent(quint16 src, quint16 module);
    void checkCodePresent(quint16 src, quint16 module, quint16 code);
    void requestAuth(quint16 src, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails, QString authReason);

signals:
    void addressChanged();
    void readPortChanged();
    void buttonStringChanged();
    void inputsLockedChanged();
    void showConnectedChanged();
    void lockConnectButtonChanged();

private:
    static QString settingsFile;
    KijangReadClient m_readClient;
    KijangWriteClient m_writeClient;
    QString m_address;
    QString m_buttonString;
    quint16 m_readPort;
    quint16 m_writePort; // Only accessed from within class, no need getter and setter
    quint32 m_clientID;
    bool m_inputsLocked;
    bool m_showConnected; // For the 'Connect' / 'Disconnect' button
    bool m_lockConnectButton;

    QString m_serverName;
    QString m_serverVersion;

    KijangServerControlModule *moduleFFFF = nullptr; // Special case needed as special slots needed for disconnection / server name and version
    QMap<quint16, KijangClientModule *> moduleMap;
};

#endif // KIJANGCLIENTCONTROLLER_H
