#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "application/pilandok.h"
#include "application/pilandoklogger.h"

int main(int argc, char *argv[])
{
    Pilandok pilandok;
    int response = pilandok.run(argc, argv);
    qDebug() << "Response code: " << response;
    PilandokLogger::flush();
    return response;
}
