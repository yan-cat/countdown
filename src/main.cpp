#include <QApplication>
#include <QQmlApplicationEngine>
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
#include <KColorSchemeManager>
#include "debug.hpp"
#include "updater.hpp"
#include "main.hpp"
#include "tray.hpp"
#include "manager.hpp"
#include "reminder.hpp"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QCoreApplication>
#endif

QString os;
QString externalAppDataPath;

int main(int argc, char *argv[]) {
//===================================================================信息
    QCoreApplication::setOrganizationName("yancat");
    QCoreApplication::setApplicationName("Countdown");
    QCoreApplication::setApplicationVersion(APP_VERSION);

    debug().logStartup("启动");

    // 定义系统
    #ifdef Q_OS_WIN
    os = "win";
    #elif defined(Q_OS_ANDROID)
    os = "android";
    #elif defined(Q_OS_LINUX)
    os = "linux";
    #else
    os = "unknow";
    #endif

    // 定义安卓外部 data
    #if defined(Q_OS_ANDROID)
    QJniObject ctx = QNativeInterface::QAndroidApplication::context();
    if (ctx.isValid()) {
        // 传 null 拿根目录：/Android/data/<包名>/files
        QJniObject extDir = ctx.callObjectMethod(
            "getExternalFilesDir",
            "(Ljava/lang/String;)Ljava/io/File;",
            nullptr);
        if (extDir.isValid()) {
            externalAppDataPath = extDir.toString();
        } else {
            qWarning() << "getExternalFilesDir 返回无效";
        }
    } else {
        qWarning() << "无法获取 Android context";
    }
    #else
    externalAppDataPath = "";
    #endif

    debug().logStartup("定义信息");

//===================================================================Debug

    //显示日志吗
    QString rules;

    qint64 outputDebuglog = debug().getDebugOn("outputDebuglog");
    if (outputDebuglog == 0) {
        rules += "*.debug=false\n";
        qInfo() << "debug日志为关";
    }
    else if (outputDebuglog == 1) {
        rules += "Countdown.app.debug=true\n";
        qInfo() << "debug日志为仅app";
    }
    else if (outputDebuglog == 2) {
        QLoggingCategory::setFilterRules("*.debug=true");
        rules += "*.debug=true\n";
        qInfo() << "debug日志为开";
    }

    if (debug().getDebugOn("disableQmlWarn")) rules += "*.warning=false\n";
    else rules += "*.warning=true\n";

    QLoggingCategory::setFilterRules(rules); // 设置日志规则

    debug().installFileLogger(); // 启动文件日志

    debug().logStartup("初始化日志");
//===================================================================后续启动

    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false); // 关窗口不关软件

    debug().logStartup("初始化 Qt 实例");

    KColorSchemeManager::instance(); // 初始化颜色管理

    // 安卓不要初始化主题，直接加入列表
    #ifndef Q_OS_ANDROID
    KIconTheme::initTheme();
    #endif

    #ifdef Q_OS_WIN
    QApplication::setStyle("breeze");                        // QStyle 用 Breeze
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop")); // QQC2 样式用 org.kde.desktop
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths()
                               << QCoreApplication::applicationDirPath() + "/icons");
    #elif defined(Q_OS_ANDROID)
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
    // 不设 themeName！让 Kirigami 自己选 "breeze-internal"
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths()
                               << QStringLiteral("assets:/qml/org/kde/kirigami"));
    // 不要写 setFallbackSearchPaths，会干扰
    #endif

    debug().logStartup("初始化主题");

    QQmlApplicationEngine engine;

    debug().logStartup("初始化 QML 引擎");

    // 软件logo
    app.setWindowIcon(QIcon(QStringLiteral(
        ":/qt/qml/com/countdown/src/resources/icon/com.countdown.svg")));
    app.setDesktopFileName(QStringLiteral("com.countdown"));

    //翻译
    QTranslator translator;
    QString locale;
    if (debug().getDebugOn("useEnLang")) {
        locale = "en_US"; // 强制英语
        qCDebug(CountdownLog) << "强制语言为英语";
    }
    else {
        locale = QLocale::system().name(); // 按照系统
        qCDebug(CountdownLog) << "使用系统语言";
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

    debug().logStartup("初始化翻译");

//===================================================================参数

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main", "倒数日"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption minimized({"start-minimized", "minimized", "m"}, QCoreApplication::translate("main", "以最小化窗口启动软件"));
    parser.addOption(minimized);

    QCommandLineOption trayRun({"start-in-tray", "tray", "t"}, QCoreApplication::translate("main", "以隐藏窗口到托盘启动"));
    parser.addOption(trayRun);

    debug().logStartup("初始化参数");

//===================================================================单实例锁

    #ifndef Q_OS_ANDROID
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
    #endif

    debug().logStartup("初始化单实例锁");

//===================================================================安卓返回关窗口

#ifdef Q_OS_ANDROID
    class BackKeyFilter : public QObject {
    public:
        explicit BackKeyFilter(QWindow *mainWindow, QObject *parent = nullptr)
            : QObject(parent), m_mainWindow(mainWindow) {
            // 监听窗口显示/隐藏，维护栈
            connect(qApp, &QGuiApplication::focusWindowChanged, this, [this](QWindow *w) {
                if (!w || w == m_mainWindow) return;
                if (!m_windowStack.contains(w))
                    m_windowStack.append(w);
            });
        }

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::KeyRelease) {
                auto *ke = static_cast<QKeyEvent*>(event);
                if (ke->key() == Qt::Key_Back) {
                    // 从栈顶往下找第一个可见的窗口
                    while (!m_windowStack.isEmpty()) {
                        QWindow *w = m_windowStack.takeLast();
                        if (w->isVisible()) {
                            qCDebug(CountdownLog) << "检测到返回键，隐藏子窗口：" << w;
                            w->hide();
                            return true;
                        }
                    }
                }
            }
            return QObject::eventFilter(obj, event);
        }

    private:
        QWindow *m_mainWindow = nullptr;
        QList<QWindow*> m_windowStack;
    };

    debug().logStartup("初始化返回键关窗口");
#endif

//===================================================================后续启动

    parser.process(app);

    engine.loadFromModule("com.countdown", "Main");
    if (engine.rootObjects().isEmpty()) {
        qFatal("QML 引擎加载失败");
    }
    else {
        #ifdef Q_OS_ANDROID
        QWindow *mainWindow = qobject_cast<QWindow*>(engine.rootObjects().constFirst());
        auto *backFilter = new BackKeyFilter(mainWindow, &app);
        app.installEventFilter(backFilter);
        #endif
    }

    debug().logStartup("加载 QML");

    updater().getReleaseInfo(); // 检查更新

//===================================================================托盘

    tray().trayInit();

    QObject *root = engine.rootObjects().constFirst();
    if (auto *window = qobject_cast<QQuickWindow*>(root)) {
        QObject::connect(window, &QQuickWindow::closing, qApp,
                        [](QQuickCloseEvent *) {
                            qCDebug(CountdownLog) << "主窗口关闭";
                            if (!manager().hasSetting("closeToTray")) {
                                manager().setSetting("closeToTray", true);
                                reminder().pushReminder("倒数日", "已最小化到托盘");
                            }
                            if (!manager().setting("closeToTray", false)) QCoreApplication::quit();
                        });
    }

//===================================================================启动参数

    if (parser.isSet(minimized)) {
        qInfo() << "最小化启动";
        QObject *root = engine.rootObjects().constFirst();
        if (auto *window = qobject_cast<QQuickWindow*>(root)) {
            window->showMinimized();
        }
    }

    if (parser.isSet(trayRun)) {
        qInfo() << "隐藏窗口到托盘启动";
        QObject *root = engine.rootObjects().constFirst();
        if (auto *window = qobject_cast<QQuickWindow*>(root)) {
            window->close();
        }
    }

//===================================================================APP退出

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        tray().shutdown();
        qInfo() << "正常退出";
    });

    return app.exec();
}
