#ifndef PILANDOKOUTPUTMANAGER_H
#define PILANDOKOUTPUTMANAGER_H

#include <QObject>

class PilandokOutputManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool allowLocalWebcam READ allowLocalWebcam WRITE setAllowLocalWebcam NOTIFY allowLocalWebcamChanged)
public:
    explicit PilandokOutputManager(QObject *parent = nullptr);

    bool allowLocalWebcam() const;
    void setAllowLocalWebcam(bool newAllowLocalWebcam);

private:
    bool m_allowLocalWebcam;

signals:
    void allowLocalWebcamChanged();

};

#endif // PILANDOKOUTPUTMANAGER_H
