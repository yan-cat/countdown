#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLockFile>
#include <QStandardPaths>
#include <QDir>
#include "manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("yancat");
    QCoreApplication::setApplicationName("Countdown");
    QCoreApplication::setApplicationVersion("1.1");
//===================================================================锁
    const QString lockDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(lockDir);

    QLockFile lock(lockDir + "/single.lock");
    if (!lock.tryLock(100)) {
        qWarning() << "已经有一个实例在运行";
        return 0;
    }
//===================================================================后续启动
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    CountdownManager manager;
    engine.rootContext()->setContextProperty("manager", &manager);

    engine.loadFromModule("com.countdown", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}