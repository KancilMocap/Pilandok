#ifndef PILANDOK_H
#define PILANDOK_H

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <QSettings>
#include <QVersionNumber>
#include <QSharedPointer>
#include <QtQml>
#include <kijangprotocol.h>
#include "pilandoklogger.h"
#include "../network/kijangclient.h"
#include "../output/pilandokoutputmanager.h"

class Pilandok : public QObject
{
    Q_OBJECT
public:
    explicit Pilandok(QObject *parent = nullptr);
    ~Pilandok();

    int run(int argc, char** argv);

    const KijangClient &client() const;

private:
    KijangClient m_client;
    PilandokOutputManager m_outputManager;
    QSharedPointer<QQmlApplicationEngine> m_engine;

signals:

public slots:

};

#endif // PILANDOK_H
