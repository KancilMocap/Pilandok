#ifndef KIJANGCLIENT_H
#define KIJANGCLIENT_H

#include <QObject>

class KijangClient : public QObject
{
    Q_OBJECT
public:
    explicit KijangClient(QObject *parent = nullptr);

signals:

};

#endif // KIJANGCLIENT_H
