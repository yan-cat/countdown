#include <QSettings>
#include <QFile>
#include <QMutex>
#include <QStandardPaths>
#include <QDir>
#include <QElapsedTimer>
#include "debug.hpp"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QCoreApplication>
#endif

Q_LOGGING_CATEGORY(CountdownLog, "Countdown.app")

namespace {
QString &logPath() {
    static QString path;
    return path;
}
bool showLogSource() {
    static bool value = debug().getDebugOn("showLogSource");
    return value;
}
bool showStartupDuration() {
    static bool value = debug().getDebugOn("showStartupDuration");
    return value;
}

// 启动计时
QElapsedTimer &startupTimer() {
    static QElapsedTimer t;
    return t;
}
bool &startupStarted() {
    static bool started = false;
    return started;
}
}

// 初始化函数与统一实例
CountdownDebug::CountdownDebug(QObject *parent) : QObject(parent) { }
CountdownDebug *CountdownDebug::create(QQmlEngine *, QJSEngine *) {
    CountdownDebug *m = &debug();                      // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}
CountdownDebug &debug() {
    static CountdownDebug instance(nullptr);
    return instance;
}

// 获取debug设置
qint64 CountdownDebug::getDebugOn(const QString &key) {
    QSettings settings;
    if (settings.value("debugMode", false).toBool()){
        qint64 s = settings.value(key, false).toInt();
        qCDebug(CountdownLog) << "获取debug状态值：" << key << "，值：" << s;
        return s;
    }
    else return false;
}

// 日志写文件
static QFile &logFile() {
    static QFile file;
    return file;
}
static QMutex &logMutex() {
    static QMutex mutex;
    return mutex;
}
// 自定义消息处理器
static void fileMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context) // 不要还不让删

    QMutexLocker locker(&logMutex());

    // 时间戳
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 类型标签
    QString typeStr;
    switch (type) {
    case QtDebugMsg:    typeStr = "DEBUG";    break;
    case QtInfoMsg:     typeStr = "INFO";     break;
    case QtWarningMsg:  typeStr = "WARNING";  break;
    case QtCriticalMsg: typeStr = "CRITICAL"; break;
    case QtFatalMsg:    typeStr = "FATAL";    break;
    }

    QString file = context.file ? QString::fromUtf8(context.file) : "unknown";
    if (showLogSource()) file = "[" + QFileInfo(file).fileName() + "] ";
    else file = "";

    QString line = QString("[%1] [%2] %3%4\n")
                       .arg(time, typeStr, file ,msg);

    // 写文件
    if (logFile().isOpen()) {
        QTextStream out(&logFile());
        out << line;
        out.flush();   // 立即落盘，崩溃时也能保留
    }

    // 同时保留控制台输出
    fprintf(stderr, "%s", line.toLocal8Bit().constData());
    fflush(stderr);

    // fatal 时终止
    if (type == QtFatalMsg) {
        abort();
    }
}

// 安装消息处理器
void CountdownDebug::installFileLogger() {
    QString logDir;

    #ifdef Q_OS_ANDROID
    QJniObject ctx = QNativeInterface::QAndroidApplication::context();
    QJniObject extDir = ctx.callObjectMethod(
        "getExternalFilesDir",
        "(Ljava/lang/String;)Ljava/io/File;",
        QJniObject::fromString("logs").object<jstring>());
    if (extDir.isValid()) {
        logDir = extDir.toString();
        qInfo() << "Android external log dir:" << logDir;
    } else {
        qWarning() << "getExternalFilesDir invalid, fallback to AppDataLocation";
        logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    }
    #else
    logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    #endif

    QDir().mkpath(logDir);

    logPath() = logDir + "/Countdown.log";
    logFile().setFileName(logPath());

    if (getDebugOn("outputLogFile")) {
        if (!logFile().open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "无法打开日志文件：" << logPath();
            return;
        }
    }

    qInstallMessageHandler(fileMessageHandler);
}

// 关闭日志
void CountdownDebug::closeFileLogger() {
    qInstallMessageHandler(nullptr);
    if (logFile().isOpen()) {
        logFile().close();
    }
}

QString CountdownDebug::getLogs() {
    QFile file(logPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(file.readAll());
}

// 清空日志
void CountdownDebug::clearLogs() {
    QFile file(logPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "清空日志失败:" << file.errorString();
    } else {
        file.close();
    }
}

void CountdownDebug::logStartup(const QString &stage) {
    if (!showStartupDuration()) return;

    if (!startupStarted()) {
        startupTimer().start();
        startupStarted() = true;
        qCDebug(CountdownLog) << "[启动] 开始计时";
        return;
    }
    qCDebug(CountdownLog) << "[启动]" << stage << ":" << startupTimer().restart() << "ms";
}
