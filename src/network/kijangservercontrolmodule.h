#ifndef KIJANGSERVERCONTROLMODULE_H
#define KIJANGSERVERCONTROLMODULE_H

#include "kijangclientmodule.h"
#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QLoggingCategory>

class KijangServerControlModule : public QObject, public KijangClientModule
{
    Q_OBJECT
    Q_INTERFACES(KijangClientModule)
public:
    explicit KijangServerControlModule(QObject *parent = nullptr);

signals:
    void clientIDReceived(quint32 id);
    void serverName(QString name);
    void serverVersion(QString version);
    void serverRequestDisconnect();

    // KijangClientModule interface
public:
    void handleResponse(KijangProtocol res) Q_DECL_OVERRIDE;
    quint16 module() Q_DECL_OVERRIDE;
    QString description() Q_DECL_OVERRIDE;
    void handleLocalResponse(quint16 src, KijangProtocol req) Q_DECL_OVERRIDE;
    void setClientID(quint32 client) Q_DECL_OVERRIDE;
    bool canHandleCode(quint16 code) Q_DECL_OVERRIDE;
    void modulePresent(quint16 module, bool present) Q_DECL_OVERRIDE;
    void codePresent(quint16 module, quint16 code, bool present) Q_DECL_OVERRIDE;
    void onStart() Q_DECL_OVERRIDE;
    void onStop() Q_DECL_OVERRIDE;
    void setServerName(QString name) Q_DECL_OVERRIDE;
    void setServerVersion(QString version) Q_DECL_OVERRIDE;
    void authFail(AuthFailureType failureType, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails) Q_DECL_OVERRIDE;

signals:
    void sendRequest(KijangProtocol request) Q_DECL_OVERRIDE;
    void sendLocalRequest(quint16 src, quint16 target, KijangProtocol req) Q_DECL_OVERRIDE;
    void checkModulePresent(quint16 src, quint16 module) Q_DECL_OVERRIDE;
    void checkCodePresent(quint16 src, quint16 module, quint16 code) Q_DECL_OVERRIDE;
    void requestAuth(quint16 src, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails, QString authReason) Q_DECL_OVERRIDE;

private:
    void pingServer();
    void pongReceived(quint64 requestID);
    void requestServerInfo();
    void handleSystemInfoRequest(bool isRemote, quint16 src, KijangProtocol req);
    void sendSystemInfoRequests();

    quint32 m_clientID;
    QMutex pingRecordsMutex;
    QMap<quint64, qint64> pingRecords;

    QString motherboardInfo;
    QString cpuInfo;
    QString gpuInfo;
    QString hardDiskInfo;
    QString ramInfo;
    QString peripheralInfo;
    QString kernelName;
    QString kernelVersion;
    QString osType;
    QString osVersion;

};

#endif // KIJANGSERVERCONTROLMODULE_H
