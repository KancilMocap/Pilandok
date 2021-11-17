#include "pilandok.h"

Q_DECLARE_LOGGING_CATEGORY(application);
Q_LOGGING_CATEGORY(application,"application");

Pilandok::Pilandok(QObject *parent) : QObject(parent)
{

}

Pilandok::~Pilandok()
{

}

int Pilandok::run(int argc, char **argv)
{
    qRegisterMetaType<Kijang::KijangProtocol>();

    // Set application info
    QCoreApplication::setApplicationName("Pilandok");
    QCoreApplication::setOrganizationName("KancilMocap");
    QCoreApplication::setOrganizationDomain("kancil.moe");
    QVersionNumber appVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_MIC);
    QString versionString = appVersion.toString();
#ifdef QT_DEBUG
    QDateTime dateTime = QDateTime::currentDateTime();
    versionString += "-debug." + QString::number(VERSION_DEBUG) + "+" + dateTime.toString("yyyyMMddhhmmss");
#endif
    QCoreApplication::setApplicationVersion(versionString);

    // Attach logger
    PilandokLogger::attach();
    qInfo(application) << "Starting" << QCoreApplication::applicationName();
    qInfo(application) << "Current version:" << versionString;

    // Loads home page
    QGuiApplication app(argc, argv);
    m_engine = QSharedPointer<QQmlApplicationEngine>(new QQmlApplicationEngine());
    PilandokLogger logger;
    m_engine->rootContext()->setContextProperty("pilandokApp", this);
    m_engine->rootContext()->setContextProperty("pilandokLogsUI", &logger);
    m_engine->rootContext()->setContextProperty("kijangClient", &m_client);
    m_engine->rootContext()->setContextProperty("pilandokOutputManager", &m_outputManager);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(m_engine.data(), &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    m_engine->load(url);

    return app.exec();
}

const KijangClient &Pilandok::client() const
{
    return m_client;
}
