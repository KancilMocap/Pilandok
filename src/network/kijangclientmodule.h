#ifndef KIJANGCLIENTMODULE_H
#define KIJANGCLIENTMODULE_H

#include <QObject>
#include <kijangprotocol.h>

class KijangClientModule : public QObject
{
    Q_OBJECT
public:
    explicit KijangClientModule(QObject *parent = nullptr);
    virtual void handleResponse(Kijang::KijangProtocol response) = 0;
    quint32 module() const;

signals:
    void sendRequest(Kijang::KijangProtocol request);

protected:
    quint32 m_module;

};

#endif // KIJANGCLIENTMODULE_H
