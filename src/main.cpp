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
    CountdownManager manager;

//===================================================================Debug

    //显示详细日志吗
    if (manager.getDebugOn("outputAllDebuglog")) QLoggingCategory::setFilterRules("*.debug=true");
    else QLoggingCategory::setFilterRules("*.debug=false");

    //显示app日志吗
    if (manager.getDebugOn("outputDebuglog")) QLoggingCategory::setFilterRules("Countdown.app.debug=true");
    else QLoggingCategory::setFilterRules("Countdown.app.debug=false");

//===================================================================后续启动

    parser.process(app);
    engine.rootContext()->setContextProperty("manager", &manager);
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