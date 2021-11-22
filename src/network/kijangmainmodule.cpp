#include "kijangmainmodule.h"

Q_DECLARE_LOGGING_CATEGORY(mainModule);
Q_LOGGING_CATEGORY(mainModule,"clientModule-FFFE");

KijangMainModule::KijangMainModule(QObject *parent) : QObject(parent)
{

}

void KijangMainModule::handleResponse(KijangProtocol response)
{
    // TODO: Complete
}

quint16 KijangMainModule::module()
{
    return 65534; // FFFE
}

QString KijangMainModule::description()
{
    return "Kijang main module";
}

void KijangMainModule::handleLocalResponse(quint16 src, KijangProtocol req)
{
    // TODO: Complete
}

bool KijangMainModule::canHandleCode(quint16 code)
{
    qWarning(mainModule) << description() << "is only meant for remote access only";
    return false;
}

void KijangMainModule::modulePresent(quint16 module, bool present)
{
}

void KijangMainModule::codePresent(quint16 module, quint16 code, bool present)
{
}

void KijangMainModule::setClientID(quint32 client)
{
    m_clientID = client;
}

void KijangMainModule::setServerName(QString name)
{
}

void KijangMainModule::setServerVersion(QString version)
{
}

void KijangMainModule::onStart()
{
    KijangProtocol request(m_clientID, 32766, 5); // 7FFE, 0005
    emit sendRequest(request);
}

void KijangMainModule::onStop()
{

}

void KijangMainModule::authFail(AuthFailureType failureType, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails)
{
    // TODO: Complete
}
