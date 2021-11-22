#ifndef KIJANGREADCLIENT_H
#define KIJANGREADCLIENT_H

#include "kijangclient.h"

class KijangReadClient : public KijangClient
{
    Q_OBJECT
public:
    explicit KijangReadClient(QObject *parent = nullptr);
};

#endif // KIJANGREADCLIENT_H
