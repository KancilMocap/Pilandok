#ifndef PILANDUK_H
#define PILANDUK_H

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <QSettings>
#include <QVersionNumber>
#include <QSharedPointer>
#include <QtQml>
#include <kijangprotocol.h>
#include "pilanduklogger.h"
#include "../network/kijangclient.h"
#include "../output/pilandukoutputmanager.h"

class Pilanduk : public QObject
{
    Q_OBJECT
public:
    explicit Pilanduk(QObject *parent = nullptr);
    ~Pilanduk();

    int run(int argc, char** argv);

    const KijangClient &client() const;

private:
    KijangClient m_client;
    PilandukOutputManager m_outputManager;
    QSharedPointer<QQmlApplicationEngine> m_engine;

signals:

public slots:

};

#endif // PILANDUK_H
