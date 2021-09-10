#ifndef KIJANGMOTIONINPUT_H
#define KIJANGMOTIONINPUT_H

#include <QObject>

class KijangMotionInput : public QObject
{
    Q_OBJECT
public:
    explicit KijangMotionInput(QObject *parent = nullptr);

signals:

};

#endif // KIJANGMOTIONINPUT_H
