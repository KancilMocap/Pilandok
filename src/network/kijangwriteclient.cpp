#include "kijangwriteclient.h"

KijangWriteClient::KijangWriteClient(QObject *parent) : KijangClient(parent)
{

}

void KijangWriteClient::sendRequest(KijangProtocol request) {
    socket.write(request.toByteArray());
    socket.waitForBytesWritten();
}
