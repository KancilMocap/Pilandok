#ifndef KIJANGCLIENTMODULE_H
#define KIJANGCLIENTMODULE_H

#include <QtPlugin>
#include "kijangprotocol.h"

class KijangClientModule
{
    Q_DISABLE_COPY(KijangClientModule)
public:
    explicit KijangClientModule() = default;
    virtual ~KijangClientModule() {}
    virtual quint16 module() = 0;
    virtual QString description() = 0;

    enum AuthFailureType {
        CANCELLED,
        INCORRECT_AUTH_METHOD,
        ACCESS_BLOCKED,
        NO_SERVER_VERSION,
        INVALID_PARAMETERS
    };

    // Handle response from the server
    virtual void handleResponse(KijangProtocol res) = 0;
    // Handle requests from other modules
    virtual void handleLocalResponse(quint16 src, KijangProtocol req) = 0;
    // Whether this module can handle the specific code
    virtual bool canHandleCode(quint16 code) = 0;
    // Slot - whether a module is present
    virtual void modulePresent(quint16 module, bool present) = 0;
    // Slot - Whether the module can handle the selected code
    virtual void codePresent(quint16 module, quint16 code, bool present) = 0;
    // Slot - Set the ID of the client
    virtual void setClientID(quint32 client) = 0;
    // Slot - Sets the name of the server
    virtual void setServerName(QString name) = 0;
    // Slot - Sets the version of the server
    virtual void setServerVersion(QString version) = 0;
    // Listener - Runs once client connected
    virtual void onStart() = 0;
    // Listener - Runs after client disconnected
    virtual void onStop() = 0;
    // Slot - On auth failure
    virtual void authFail(AuthFailureType failureType, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails) = 0;

signals:
    // Send a request to the server
    virtual void sendRequest(KijangProtocol request)= 0;
    // Send packet to a local module handler
    virtual void sendLocalRequest(quint16 src, quint16 target, KijangProtocol req) = 0;
    // Whether a specific module is present
    virtual void checkModulePresent(quint16 src, quint16 module) = 0;
    // Whether a a specific module can handle a specific code
    virtual void checkCodePresent(quint16 src, quint16 module, quint16 code) = 0;
    // Request for authentication information from the user and sends the request
    virtual void requestAuth(quint16 src, quint16 module, quint16 code, quint16 authMethod, QByteArray authDetails, QString authReason) = 0;
};

Q_DECLARE_INTERFACE(KijangClientModule, "moe.kancil.pilandok.network.modulehandler");

#endif // KIJANGCLIENTMODULE_H
