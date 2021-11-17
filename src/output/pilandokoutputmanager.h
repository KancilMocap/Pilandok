#ifndef PILANDOKOUTPUTMANAGER_H
#define PILANDOKOUTPUTMANAGER_H

#include <QObject>
#include <QDir>
#include <QSettings>

class PilandokOutputManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool allowLocalWebcam READ allowLocalWebcam WRITE setAllowLocalWebcam NOTIFY allowLocalWebcamChanged)
public:
    explicit PilandokOutputManager(QObject *parent = nullptr);
    ~PilandokOutputManager();

    bool allowLocalWebcam() const;
    void setAllowLocalWebcam(bool newAllowLocalWebcam);

private:
    static QString settingsFile;
    bool m_allowLocalWebcam;

signals:
    void allowLocalWebcamChanged();

};

#endif // PILANDOKOUTPUTMANAGER_H
