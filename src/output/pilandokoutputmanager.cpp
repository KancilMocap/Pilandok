#include "pilandokoutputmanager.h"

PilandokOutputManager::PilandokOutputManager(QObject *parent) : QObject(parent)
{

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
