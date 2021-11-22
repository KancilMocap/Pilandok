#ifndef KIJANGMAINMODULE_H
#define KIJANGMAINMODULE_H

#include "kijangclientmodule.h"
#include <QObject>
#include <QLoggingCategory>

class KijangMainModule : public QObject, KijangClientModule
{
    Q_OBJECT
public:
    explicit KijangMainModule(QObject *parent = nullptr);

    // KijangClientModule interface
public:
    void handleResponse(KijangProtocol response) Q_DECL_OVERRIDE;

    // KijangClientModule interface
public:
    quint16 module() Q_DECL_OVERRIDE;
    QString description() Q_DECL_OVERRIDE;
    void handleLocalResponse(quint16 src, KijangProtocol req) Q_DECL_OVERRIDE;
    bool canHandleCode(quint16 code) Q_DECL_OVERRIDE;
    void modulePresent(quint16 module, bool present) Q_DECL_OVERRIDE;
    void codePresent(quint16 module, quint16 code, bool present) Q_DECL_OVERRIDE;
    void setClientID(quint32 client) Q_DECL_OVERRIDE;
    void setServerName(QString name) Q_DECL_OVERRIDE;
    void setServerVersion(QString version) Q_DECL_OVERRIDE;
    void onStart() Q_DECL_OVERRIDE;
    void onStop() Q_DECL_OVERRIDE;
    void authFail(AuthFailureType failureType, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails) Q_DECL_OVERRIDE;

signals:
    void sendRequest(KijangProtocol request) Q_DECL_OVERRIDE;
    void sendLocalRequest(quint16 src, quint16 target, KijangProtocol req) Q_DECL_OVERRIDE;
    void checkModulePresent(quint16 src, quint16 module) Q_DECL_OVERRIDE;
    void checkCodePresent(quint16 src, quint16 module, quint16 code) Q_DECL_OVERRIDE;
    void requestAuth(quint16 src, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails, QString authReason) Q_DECL_OVERRIDE;

private:
    quint32 m_clientID;

};

#endif // KIJANGMAINMODULE_H
