#include "pilandokoutputmanager.h"

QString PilandokOutputManager::settingsFile = QDir::currentPath() + QDir::separator() + "settings.ini";

PilandokOutputManager::PilandokOutputManager(QObject *parent) : QObject(parent)
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("output");
    setAllowLocalWebcam(settings.value("allowDevices", true).toBool());
    settings.endGroup();
}

PilandokOutputManager::~PilandokOutputManager()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("output");
    settings.setValue("allowDevices", m_allowLocalWebcam);
    settings.endGroup();
    settings.sync();
}

bool PilandokOutputManager::allowLocalWebcam() const
{
    return m_allowLocalWebcam;
}

void PilandokOutputManager::setAllowLocalWebcam(bool newAllowLocalWebcam)
{
    if (m_allowLocalWebcam == newAllowLocalWebcam)
        return;
    m_allowLocalWebcam = newAllowLocalWebcam;
    emit allowLocalWebcamChanged();
}
