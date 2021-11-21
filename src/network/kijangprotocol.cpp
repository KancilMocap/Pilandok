#include "kijangprotocol.h"

KijangProtocol::KijangProtocol()
{
    m_clientID = 0;
    m_module = 0;
    m_code = 0;
    initDefault();
}

KijangProtocol::KijangProtocol(quint32 client)
{
    m_clientID = client;
    m_module = 0;
    m_code = 0;
    initDefault();
}

KijangProtocol::KijangProtocol(quint32 client, quint16 module)
{
    m_clientID = client;
    m_module = 0;
    m_code = 0;
    initDefault();
}

KijangProtocol::KijangProtocol(quint32 client, quint16 module, quint16 code)
{
    QRandomGenerator random;
    m_clientID = client;
    m_requestID = random.generate64();
    m_module = module;
    m_code = code;
    initDefault();
}

KijangProtocol::KijangProtocol(QByteArray array)
{
    initFromArray(array, 0);
}

KijangProtocol::KijangProtocol(QByteArray array, quint32 client)
{
    initFromArray(array, client);
}

KijangProtocol::~KijangProtocol()
{

}

void KijangProtocol::initDefault()
{
    QRandomGenerator random;
    m_version = 1;
    m_requestID = random.generate64();
    m_packetCount = 1;
    m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
}

void KijangProtocol::initFromArray(QByteArray array, quint32 clientFallback)
{
    if (array.length() < 20) {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::INVALID_LENGTH;
        m_errorString = QString("Minimum length 28 expected for basic Kijang request but length %1 received").arg(array.length());
        return;
    }

    QByteArray version = array.left(4);
    m_version = qFromBigEndian<quint32>(version);
    array.remove(0, 4);

    QByteArray module = array.left(2);
    m_module = qFromBigEndian<quint16>(module);
    array.remove(0, 2);

    QByteArray code = array.left(2);
    m_code = qFromBigEndian<quint16>(code);
    array.remove(0, 2);

    QByteArray client = array.left(4);
    m_clientID = qFromBigEndian<quint32>(client);
    array.remove(0, 4);

    QByteArray request = array.left(8);
    m_requestID = qFromBigEndian<quint64>(request);
    array.remove(0, 8);

    QByteArray packet = array.left(4);
    m_packetCount = qFromBigEndian<quint32>(packet);
    array.remove(0, 4);

    QByteArray currentPacket = array.left(4);
    m_currentPacket = qFromBigEndian<quint32>(currentPacket);
    array.remove(0,4);

    if (m_version == 1) {
        m_data = array;
    } else if (array.length() < 4) {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::INVALID_LENGTH;
        m_errorString = QString("Minimum length 32 expected for Kijang request v2 but length %1 received").arg(array.length());
        return;
    } else {
        QByteArray dataSize = array.left(4);
        quint32 dataSizeInt = qFromBigEndian<quint32>(dataSize);
        array.remove(0, 4);
        if (dataSizeInt > array.length()) {
            m_exceptionInfo = KijangProtocol::ExceptionInfo::DATA_TOO_SHORT;
            m_errorString = QString("Data length stated as %1 but remaining array length is %2").arg(dataSizeInt).arg(array.length());
            return;
        } else {
            m_data = array;
        }
    }
    if (!m_clientID) m_clientID = clientFallback; // Fallback to set client ID
    // Module and code can be 0000, client ID, request ID and packet count cannot
    if (!m_clientID || !m_requestID || !m_packetCount) {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::READING_FAILED;
        m_errorString = "Invalid parameters received for one or more required segments, reading failed";
    } else {
        m_exceptionInfo = KijangProtocol::ExceptionInfo::NONE;
    }
}

quint32 KijangProtocol::version() const
{
    return m_version;
}

void KijangProtocol::setVersion(quint32 newVersion)
{
    m_version = newVersion;
}

quint32 KijangProtocol::currentPacket() const
{
    return m_currentPacket;
}

void KijangProtocol::setCurrentPacket(quint32 newCurrentPacket)
{
    m_currentPacket = newCurrentPacket;
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
    stream << m_version << m_module << m_code << m_clientID << m_requestID << m_packetCount << m_currentPacket;
    if (m_version == 2) stream << m_data.size();
    response.append(m_data);
    qDebug() << "Sending data: " << response.toHex();
    return response;
}
