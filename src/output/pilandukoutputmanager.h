#ifndef PILANDUKOUTPUTMANAGER_H
#define PILANDUKOUTPUTMANAGER_H

#include <QObject>

class PilandukOutputManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool allowLocalWebcam READ allowLocalWebcam WRITE setAllowLocalWebcam NOTIFY allowLocalWebcamChanged)
public:
    explicit PilandukOutputManager(QObject *parent = nullptr);

    bool allowLocalWebcam() const;
    void setAllowLocalWebcam(bool newAllowLocalWebcam);

private:
    bool m_allowLocalWebcam;

signals:
    void allowLocalWebcamChanged();

};

#endif // PILANDUKOUTPUTMANAGER_H
