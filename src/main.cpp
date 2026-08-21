#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "manager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    CountdownManager manager;
    engine.rootContext()->setContextProperty("manager", &manager);

    engine.loadFromModule("com.countdown", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}