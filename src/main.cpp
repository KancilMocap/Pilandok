#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "application/pilanduk.h"
#include "application/pilanduklogger.h"

int main(int argc, char *argv[])
{
    Pilanduk pilanduk;
    int response = pilanduk.run(argc, argv);
    qDebug() << "Response code: " << response;
    PilandukLogger::flush();
    return response;
}
