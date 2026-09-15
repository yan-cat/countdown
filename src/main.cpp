#include <QApplication>
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
#include <QTranslator>
#include <QIcon>
#include <QQuickStyle>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "debug.hpp"
#include "updater.hpp"
#include "reminder.hpp"

int main(int argc, char *argv[]) {
//===================================================================信息
    QCoreApplication::setOrganizationName("yancat");
    QCoreApplication::setApplicationName("Countdown");
    QCoreApplication::setApplicationVersion(APP_VERSION);

//===================================================================参数

    QCommandLineParser parser;
    parser.setApplicationDescription("倒数日");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption minimized({"start-minimized", "minimized", "m"}, "静默启动（最小化窗口启动）");
    parser.addOption(minimized);


//===================================================================后续启动

    // 安卓不要初始化主题
    #ifndef Q_OS_ANDROID
    KIconTheme::initTheme();
    #endif
    QApplication app(argc, argv);

    #ifdef Q_OS_WIN
    QApplication::setStyle("breeze");                        // QStyle 用 Breeze
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop")); // QQC2 样式用 org.kde.desktop
    #endif

    QQmlApplicationEngine engine;

    // 软件logo
    app.setWindowIcon(QIcon(QStringLiteral(
        ":/qt/qml/com/countdown/src/resources/icon/com.countdown.svg")));
    app.setDesktopFileName(QStringLiteral("com.countdown"));

    //翻译
    QTranslator translator;
    QString locale;
    if (debug().getDebugOn("useEnLang")) {
        locale = "en_US"; // 强制英语
        qCDebug(CountdownLog) << "[ Debug ]" << "强制语言为英语";
    }
    else {
        locale = QLocale::system().name(); // 按照系统
        qCDebug(CountdownLog) << "[ Debug ]" << "使用系统语言";
    }
    // 尝试加载对应的翻译文件
    if (translator.load(QString(":/i18n/countdown_%1.qm").arg(locale))) {
        app.installTranslator(&translator);
    } else {
        // 如果精确匹配失败，可以尝试只取语言部分，如 "zh"
        QString shortLocale = locale.left(locale.indexOf('_'));
        if (translator.load(QString(":/i18n/countdown_%1.qm").arg(shortLocale))) {
            app.installTranslator(&translator);
        }
    }

//===================================================================单实例锁

    const QString lockDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(lockDir);

    QLockFile lock(lockDir + "/single.lock");
    if (!lock.tryLock(100)) {
        qWarning() << "已经有一个倒数日在运行";
        QDialog dialog;
        dialog.setWindowTitle(QCoreApplication::translate("main", "倒数日"));
        dialog.resize(200, 100);
        auto *layout = new QVBoxLayout(&dialog);
        layout->addWidget(new QLabel(QCoreApplication::translate("main", "已经有一个倒数日在运行"), &dialog));
        auto *btn = new QPushButton("确定", &dialog);
        layout->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, &dialog, &QDialog::accept);
        dialog.exec();
        return 0;
    }

//===================================================================Debug

    //显示日志吗
    if (debug().getDebugOn("outputDebuglog") == 0) {
        QLoggingCategory::setFilterRules("*.debug=false");
        qInfo() << "[ Info ]" << "debug日志为关";
    }
    else if (debug().getDebugOn("outputDebuglog") == 1) {
        QLoggingCategory::setFilterRules("Countdown.app.debug=true");
        qInfo() << "[ Info ]" << "debug日志为仅app";
    }
    else if (debug().getDebugOn("outputDebuglog") == 2) {
        QLoggingCategory::setFilterRules("*.debug=true");
        qInfo() << "[ Info ]" << "debug日志为开";
    }

    if (debug().getDebugOn("disableQmlWarn")) QLoggingCategory::setFilterRules("*.warning=false");
    else QLoggingCategory::setFilterRules("*.warning=true");

//===================================================================后续启动

    parser.process(app);

    engine.loadFromModule("com.countdown", "Main");
    if (engine.rootObjects().isEmpty()) return -1;

    updater().getReleaseInfo(); // 检查更新

//===================================================================最小化启动

    if (parser.isSet(minimized)) {
        qCDebug(CountdownLog) << "[ Debug ]" << "静默启动";
        QObject *root = engine.rootObjects().constFirst();
        if (auto *window = qobject_cast<QQuickWindow*>(root)) {
            window->showMinimized();
        }
    }

//===================================================================APP退出

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        qCDebug(CountdownLog) << "[ Debug ]" << "正常退出";
    });

    return app.exec();
}
