#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "LogAnalyzer.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<LogAnalyzer>("Backend", 1, 0, "LogAnalyzer");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("DetectAnomalLog", "Main");

    return app.exec();
}
