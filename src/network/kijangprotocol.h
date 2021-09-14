#ifndef KIJANGPROTOCOL_H
#define KIJANGPROTOCOL_H

#include <QDebug>
#include <QDataStream>
#include <QIODevice>
#include <QtEndian>
#include <QRandomGenerator>

class KijangProtocol
{
public:
    KijangProtocol();
    KijangProtocol(quint32 client);
    KijangProtocol(quint32 client, quint64 request);
    KijangProtocol(quint32 client, quint64 request, quint16 module, quint16 code);
    KijangProtocol(QByteArray array);
    ~KijangProtocol();
    KijangProtocol &operator=(const KijangProtocol &) = default;

    enum ExceptionInfo {
        NONE,
        INVALID_LENGTH,
        READING_FAILED,
        CLIENT_ID_EXISTS
    };

private:
    quint16 m_module;
    quint16 m_code;
    quint32 m_clientID;
    quint64 m_requestID;
    quint32 m_packetCount;
    QByteArray m_data;
    ExceptionInfo m_exceptionInfo;
    QString m_errorString;

public:
    // To section
    QByteArray toByteArray() const;

    // Operators
    friend QDebug operator<<(QDebug dbg, const KijangProtocol &p);

    quint16 module() const;
    void setModule(quint16 newModule);
    quint16 code() const;
    void setCode(quint16 newCode);
    quint32 clientID() const;
    void setClientID(quint32 newClientID);
    quint64 requestID() const;
    void setRequestID(quint64 newRequestID);
    quint32 packetCount() const;
    void setPacketCount(quint32 newPacketCount);
    const QByteArray &data() const;
    void setData(const QByteArray &newData);
    const QString &errorString() const;
    ExceptionInfo exceptionInfo() const;
};

Q_DECLARE_METATYPE(KijangProtocol);
#endif // KIJANGPROTOCOL_H
