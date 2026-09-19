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
#include "main.hpp"

QString os;

int main(int argc, char *argv[]) {
//===================================================================信息
    QCoreApplication::setOrganizationName("yancat");
    QCoreApplication::setApplicationName("Countdown");
    QCoreApplication::setApplicationVersion(APP_VERSION);

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
//===================================================================参数

    QCommandLineParser parser;
    parser.setApplicationDescription("倒数日");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption minimized({"start-minimized", "minimized", "m"}, "静默启动（最小化窗口启动）");
    parser.addOption(minimized);


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

//===================================================================后续启动

    // 安卓不要初始化主题，直接加入列表
    #ifndef Q_OS_ANDROID
    KIconTheme::initTheme();
    #endif

    #if defined(Q_OS_WIN) //|| defined(Q_OS_ANDROID)
    QApplication::setStyle("breeze");                        // QStyle 用 Breeze
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop")); // QQC2 样式用 org.kde.desktop
    #endif
    #ifdef Q_OS_ANDROID
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
    #endif

    QApplication app(argc, argv);
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

//===================================================================单实例锁

    #ifdef Q_OS_ANDROID
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

    // 捕获报错
    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError> &warnings) {
                         for (const auto &err : warnings) {
                             qCritical() << "QML 警告：" << err.toString();
                         }
                     });

    updater().getReleaseInfo(); // 检查更新

//===================================================================最小化启动

    if (parser.isSet(minimized)) {
        qInfo() << "静默启动";
        QObject *root = engine.rootObjects().constFirst();
        if (auto *window = qobject_cast<QQuickWindow*>(root)) {
            window->showMinimized();
        }
    }

//===================================================================APP退出

    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        qInfo() << "正常退出";
    });

    return app.exec();
}
