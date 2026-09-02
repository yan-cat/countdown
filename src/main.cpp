#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLockFile>
#include <QStandardPaths>
#include <QDir>
#include <KIconTheme>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLoggingCategory>
#include "manager.h"
#include "updater.h"
#include "debug.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("yancat");
    QCoreApplication::setApplicationName("Countdown");
    QCoreApplication::setApplicationVersion(APP_VERSION);

//===================================================================锁

    const QString lockDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(lockDir);

    QLockFile lock(lockDir + "/single.lock");
    if (!lock.tryLock(100)) {
        qWarning() << "已经有一个实例在运行";
        return 0;
    }
//===================================================================参数

    QCommandLineParser parser;
    parser.setApplicationDescription("倒数日");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption minimized({"start-minimized", "minimized", "m"}, "静默启动（最小化窗口启动）");
    parser.addOption(minimized);

//===================================================================后续启动

    KIconTheme::initTheme();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    //链接的可调用类
    CountdownManager manager;
    CountdownUpdater updater;

//===================================================================Debug

    //显示日志吗
    if (getDebugOn("outputDebuglog") == 0)
    {
        QLoggingCategory::setFilterRules("*.debug=false");
        qInfo() << "[ Info ]" << "debug日志为关";
    }
    else if (getDebugOn("outputDebuglog") == 1)
    {
        QLoggingCategory::setFilterRules("Countdown.app.debug=true");
        qInfo() << "[ Info ]" << "debug日志为仅app";
    }
    else if (getDebugOn("outputDebuglog") == 2)
    {
        QLoggingCategory::setFilterRules("*.debug=true");
        qInfo() << "[ Info ]" << "debug日志为开";
    }

//===================================================================后续启动

    parser.process(app);

    //链接
    engine.rootContext()->setContextProperty("manager", &manager);
    engine.rootContext()->setContextProperty("updater", &updater);

    engine.loadFromModule("com.countdown", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

//===================================================================最小化启动

    if (parser.isSet(minimized)) {
        qCDebug(CountdownLog) << "[ Debug ]" << "静默启动";
        QObject *root = engine.rootObjects().constFirst();
        if (auto *window = qobject_cast<QQuickWindow*>(root)) {
            window->showMinimized();
        }
    }

    return app.exec();
}