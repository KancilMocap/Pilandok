#ifndef PILANDUKLOGGER_H
#define PILANDUKLOGGER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <iostream>
#include <QTextStream>

class PilandukLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString logString READ logString WRITE setLogString NOTIFY logStringChanged)
public:
    explicit PilandukLogger(QObject *parent = nullptr);
    static void attach();
    static void handler(QtMsgType type, const QMessageLogContext &context, const QString & msg);

    static const QString &logString();
    static void setLogString(const QString &newLogString);

signals:
    void logStringChanged();

public slots:
    static void flush();

private:
    static QString rootDir;
    static QString filename;
    static QStringList cachedLogs;
    static int cacheCount;
    static int exportCacheCount;
    static qint64 lastExportTime;
    static int minExportInterval;
    static QString m_logString;

};

#endif // PILANDUKLOGGER_H
