#include "pilandukoutputmanager.h"

PilandukOutputManager::PilandukOutputManager(QObject *parent) : QObject(parent)
{

}

bool PilandukOutputManager::allowLocalWebcam() const
{
    return m_allowLocalWebcam;
}

void PilandukOutputManager::setAllowLocalWebcam(bool newAllowLocalWebcam)
{
    if (m_allowLocalWebcam == newAllowLocalWebcam)
        return;
    m_allowLocalWebcam = newAllowLocalWebcam;
    emit allowLocalWebcamChanged();
}
