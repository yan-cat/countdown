#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include "autostart.hpp"
#include "debug.hpp"
#include "main.hpp"

// 初始化函数与统一实例
CountdownAutoStart::CountdownAutoStart(QObject *parent) : QObject(parent) { }
CountdownAutoStart *CountdownAutoStart::create(QQmlEngine *, QJSEngine *) {
    CountdownAutoStart *m = &autostart();                      // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}
CountdownAutoStart &autostart() {
    static CountdownAutoStart instance(nullptr);
    return instance;
}

// 获取自启状态
bool CountdownAutoStart::getAutoStart() {
    qCDebug(CountdownLog) << "检查开机自启状态";
    bool autostart = false;
    #ifdef Q_OS_WIN
    // Windows
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();
    QString stored = settings.value(appName).toString();

    if (stored.isEmpty()) {
        autostart = false;
    } else {
        // 去掉可能存在的引号
        if (stored.startsWith('"') && stored.endsWith('"') && stored.length() >= 2) {
            stored = stored.mid(1, stored.length() - 2);
        }
        QString current = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        autostart = (stored == current);
    }
    #elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QString desktopPath = autostartPath + "/" + "com.countdown.desktop";
    QFile file(desktopPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray content = file.readAll();
        autostart = content.contains(("Exec=\"" + QCoreApplication::applicationFilePath() + "\"").toUtf8());
        if(!autostart) qCDebug(CountdownLog) << "Desktop 启动路径不一致";
    }
    else qCDebug(CountdownLog) << "打开 Desktop 文件失败";

    #endif
    qCDebug(CountdownLog) << "当前" << os << "开机自启为：" << autostart;
    return autostart;
}

// 设定自启
void CountdownAutoStart::setAutoStart(bool enable) {
    qCDebug(CountdownLog) << "设定开机自启为：" << enable;
    if (getAutoStart() == enable) {
        qCDebug(CountdownLog) << "无需重复设置";
        return;
    }
    #ifdef Q_OS_WIN
    // Windows
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();  // "Countdown"

    if (enable) {
        // 路径带空格要加引号，否则 Windows 会解析错
        QString path = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        settings.setValue(appName, "\"" + path + "\"");
        qCDebug(CountdownLog) << "已写入注册表：" << path;
    } else {
        settings.remove(appName);
        qCDebug(CountdownLog) << "已移除注册表项";
    }
    settings.sync();  // 立即落盘，避免延迟写入
    #elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux
    QFile src(":/qt/qml/com/countdown/com.countdown.desktop");
    if (!src.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法读取内置 desktop 模板";
        return;
    }
    QString content = QString::fromUtf8(src.readAll());
    src.close();

    // 替换占位符
    content.replace("Exec=Countdown", "Exec=\"" + QCoreApplication::applicationFilePath() + "\"");

    // 写到目标位置
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QString desktopPath = autostartPath + "/" + "com.countdown.desktop";
    QFile dst(desktopPath);
    QDir().mkpath(autostartPath);
    if (enable) {
        // 开自启
        if (dst.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&dst);
            out << content;
            qCDebug(CountdownLog) << "启用开机自启";
        }
    }
    else {
        QFile::remove(desktopPath);   // 关自启
        qCDebug(CountdownLog) << "禁用开机自启";
    }
    #endif
}
