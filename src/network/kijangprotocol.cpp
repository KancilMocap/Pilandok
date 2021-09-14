#include "kijangprotocol.h"

KijangProtocol::KijangProtocol()
{
    QRandomGenerator random;
    m_clientID = 0;
    m_requestID = random.generate64();
    m_module = 0;
    m_code = 0;
    m_packetCount = 1;
    m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
}

KijangProtocol::KijangProtocol(quint32 client)
{
    QRandomGenerator random;
    m_clientID = client;
    m_requestID = random.generate64();
    m_module = 0;
    m_code = 0;
    m_packetCount = 1;
    m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
}

KijangProtocol::KijangProtocol(quint32 client, quint64 request)
{
    m_clientID = client;
    m_requestID = request;
    m_module = 0;
    m_code = 0;
    m_packetCount = 1;
    m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
}

KijangProtocol::KijangProtocol(quint32 client, quint64 request, quint16 module, quint16 code)
{
    m_clientID = client;
    m_requestID = request;
    m_module = module;
    m_code = code;
    m_packetCount = 1;
    m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
}

KijangProtocol::KijangProtocol(QByteArray array)
{
    if (array.length() < 20) {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::INVALID_LENGTH;
        m_errorString = QString("Minimum length 20 expected for Kijang request but length %1 received").arg(array.length());
        return;
    }

    QByteArray module = array.left(2);
    m_module = qFromBigEndian<qint16>(module);
    array.remove(0, 2);

    QByteArray code = array.left(2);
    m_code = qFromBigEndian<qint16>(code);
    array.remove(0, 2);

    QByteArray client = array.left(4);
    m_clientID = qFromBigEndian<qint32>(client);
    array.remove(0, 4);

    QByteArray request = array.left(8);
    m_requestID = qFromBigEndian<qint64>(request);
    array.remove(0, 8);

    QByteArray packet = array.left(4);
    m_packetCount = qFromBigEndian<qint32>(packet);
    array.remove(0, 4);

    m_data = array;

    // Module == 7FFF (client control), Code == 0001 (Client ID provisioning)
    if (m_module == 32767 && m_code == 1) {
        if (m_clientID) {
            m_errorString = QString("Attempting to request client ID even though client ID (%1) already exists").arg(m_clientID);
            m_exceptionInfo = KijangProtocol::ExceptionInfo::CLIENT_ID_EXISTS;
        } else {
            QRandomGenerator random;
            m_clientID = random.generate();
        }
    }
    // Module and code can be 0000, client ID, request ID and packet count cannot
    if (!m_clientID || !m_requestID || !m_packetCount) {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::READING_FAILED;
        m_errorString = "Invalid parameters received for one or more required segments, reading failed";
    } else {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
    }
}

KijangProtocol::~KijangProtocol()
{

}

KijangProtocol::ExceptionInfo KijangProtocol::exceptionInfo() const
{
    return m_exceptionInfo;
}

const QString &KijangProtocol::errorString() const
{
    return m_errorString;
}

const QByteArray &KijangProtocol::data() const
{
    return m_data;
}

void KijangProtocol::setData(const QByteArray &newData)
{
    m_data = newData;
}

quint32 KijangProtocol::packetCount() const
{
    return m_packetCount;
}

void KijangProtocol::setPacketCount(quint32 newPacketCount)
{
    m_packetCount = newPacketCount;
}

quint64 KijangProtocol::requestID() const
{
    return m_requestID;
}

void KijangProtocol::setRequestID(quint64 newRequestID)
{
    m_requestID = newRequestID;
}

quint32 KijangProtocol::clientID() const
{
    return m_clientID;
}

void KijangProtocol::setClientID(quint32 newClientID)
{
    m_clientID = newClientID;
}

quint16 KijangProtocol::code() const
{
    return m_code;
}

void KijangProtocol::setCode(quint16 newCode)
{
    m_code = newCode;
}

quint16 KijangProtocol::module() const
{
    return m_module;
}

void KijangProtocol::setModule(quint16 newModule)
{
    m_module = newModule;
}

QDebug operator<<(QDebug dbg, const KijangProtocol &p) {
    QDebugStateSaver saver(dbg);
    dbg << "KijangProtocol (" << &p << ")" << " Module: " << p.module() << "; Code: " << p.code() << "; Client ID: " << p.clientID() << "; Request ID: " << p.requestID() << "; Packet count: " << p.packetCount() << "; Data: " << QString(p.data()).toUtf8();
    return dbg;
}

QByteArray KijangProtocol::toByteArray() const {
    QByteArray response;
    QDataStream stream(&response, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << m_module << m_code << m_clientID << m_requestID << m_packetCount;
    response.append(m_data);
    return response;
}
