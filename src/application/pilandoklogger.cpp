#include "pilandoklogger.h"

QString PilandokLogger::rootDir = QDir::currentPath();
QString PilandokLogger::filename = QDir::currentPath() + QDir::separator() + "logs" + QDir::separator() + "kijang.log";
QString PilandokLogger::m_logString = "";
int PilandokLogger::cacheCount = 0;
int PilandokLogger::exportCacheCount = 200;
qint64 PilandokLogger::lastExportTime = QDateTime::currentSecsSinceEpoch();
int PilandokLogger::minExportInterval = 10;
QStringList PilandokLogger::cachedLogs = QStringList();
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

PilandokLogger::PilandokLogger(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PilandokLogger::updateLogUI);
    timer->setInterval(500);
    timer->start();
}

void PilandokLogger::attach()
{
    // Create logs folder if not exists
    QDir logsFolder(QDir::currentPath() + QDir::separator() + "logs");
    // https://stackoverflow.com/a/11517874
    if (!logsFolder.exists()) {
        logsFolder.mkpath(".");
    }
    qInstallMessageHandler(PilandokLogger::handler);
}

void PilandokLogger::handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    QString txt1;
    switch (type) {
    case QtInfoMsg:
        txt = QString("[INFO] %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("[WARNING] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("[CRITICAL] %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("[FATAL] %1").arg(msg);
        break;
    case QtDebugMsg:
        txt = QString("[DEBUG] %1").arg(msg);
        break;
    }
#ifdef QT_DEBUG
    QDir appDir(rootDir);
    txt = QDateTime::currentDateTime().toString() + " - " + txt;
    txt1 = " " + appDir.relativeFilePath(context.file) + " line: " + QString::number(context.line);
    PilandokLogger::cachedLogs << txt << txt1;
    PilandokLogger::m_logString += (txt + "\r\n");
    PilandokLogger::cacheCount += 1;
#else
    if (type != QtDebugMsg) {
        txt = QDateTime::currentDateTime().toString() + " - " + txt;
        PilandokLogger::cachedLogs << txt;
        PilandokLogger::m_logString += (txt + "\r\n");
        PilandokLogger::cacheCount += 1;
    }
#endif
    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    if (type == QtCriticalMsg || type == QtFatalMsg || (PilandokLogger::cacheCount >= 200 && (currentTime - PilandokLogger::lastExportTime > PilandokLogger::minExportInterval))) {
        PilandokLogger::flush();
    }

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);

}

void PilandokLogger::flush()
{
    QFile file(PilandokLogger::filename);
    if(file.open(QIODevice::Append) && PilandokLogger::cachedLogs.length() > 0) {
        PilandokLogger::lastExportTime = QDateTime::currentSecsSinceEpoch();
        QTextStream ts(&file);
        ts << cachedLogs.join("\r\n") << "\r\n";
        ts.flush();
        file.close();
    }
}

void PilandokLogger::updateLogUI()
{
    emit logStringChanged(m_logString);
}

void PilandokLogger::openLogsFile()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

const QString &PilandokLogger::logString()
{
    return m_logString;
}

void PilandokLogger::setLogString(const QString &newLogString)
{
    m_logString = newLogString;
}
