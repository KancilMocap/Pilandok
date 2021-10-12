#include "pilanduklogger.h"

QString PilandukLogger::rootDir = QDir::currentPath();
QString PilandukLogger::filename = QDir::currentPath() + QDir::separator() + "logs" + QDir::separator() + "kijang.log";
QString PilandukLogger::m_logString = "";
int PilandukLogger::cacheCount = 0;
int PilandukLogger::exportCacheCount = 200;
qint64 PilandukLogger::lastExportTime = QDateTime::currentSecsSinceEpoch();
int PilandukLogger::minExportInterval = 10;
QStringList PilandukLogger::cachedLogs = QStringList();
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

PilandukLogger::PilandukLogger(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PilandukLogger::updateLogUI);
    timer->setInterval(500);
    timer->start();
}

void PilandukLogger::attach()
{
    // Create logs folder if not exists
    QDir logsFolder(QDir::currentPath() + QDir::separator() + "logs");
    // https://stackoverflow.com/a/11517874
    if (!logsFolder.exists()) {
        logsFolder.mkpath(".");
    }
    qInstallMessageHandler(PilandukLogger::handler);
}

void PilandukLogger::handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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
    PilandukLogger::cachedLogs << txt << txt1;
    PilandukLogger::m_logString += (txt + "\r\n");
    PilandukLogger::cacheCount += 1;
#else
    if (type != QtDebugMsg) {
        txt = QDateTime::currentDateTime().toString() + " - " + txt;
        PilandukLogger::cachedLogs << txt;
        PilandukLogger::m_logString += (txt + "\r\n");
        PilandukLogger::cacheCount += 1;
    }
#endif
    qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    if (type == QtCriticalMsg || type == QtFatalMsg || (PilandukLogger::cacheCount >= 200 && (currentTime - PilandukLogger::lastExportTime > PilandukLogger::minExportInterval))) {
        PilandukLogger::flush();
    }

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);

}

void PilandukLogger::flush()
{
    QFile file(PilandukLogger::filename);
    if(file.open(QIODevice::Append) && PilandukLogger::cachedLogs.length() > 0) {
        PilandukLogger::lastExportTime = QDateTime::currentSecsSinceEpoch();
        QTextStream ts(&file);
        ts << cachedLogs.join("\r\n") << "\r\n";
        ts.flush();
        file.close();
    }
}

void PilandukLogger::updateLogUI()
{
    emit logStringChanged(m_logString);
}

void PilandukLogger::openLogsFile()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

const QString &PilandukLogger::logString()
{
    return m_logString;
}

void PilandukLogger::setLogString(const QString &newLogString)
{
    m_logString = newLogString;
}
