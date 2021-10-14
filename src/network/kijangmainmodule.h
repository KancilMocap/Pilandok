#ifndef KIJANGMAINMODULE_H
#define KIJANGMAINMODULE_H

#include "kijangclientmodule.h"
#include <QObject>

class KijangMainModule : public KijangClientModule
{
    Q_OBJECT
public:
    explicit KijangMainModule(QObject *parent = nullptr);

    // KijangClientModule interface
public:
    void handleResponse(Kijang::KijangProtocol response) Q_DECL_OVERRIDE;
};

#endif // KIJANGMAINMODULE_H
