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
qint64 CountdownAutoStart::getAutoStart() {
    qCDebug(CountdownLog) << "检查开机自启状态";
    qint64 autostart = 0;
    #ifdef Q_OS_WIN
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);
    QString appName = QCoreApplication::applicationName();
    QString stored = settings.value(appName).toString();

    if (stored.isEmpty()) {
        autostart = 0;
    } else {
        // 解析出路径和参数
        QString storedPath;
        QString storedArgs;

        if (stored.startsWith('"')) {
            // "路径" [参数]
            int endQuote = stored.indexOf('"', 1);
            if (endQuote > 0) {
                storedPath = stored.mid(1, endQuote - 1);
                storedArgs = stored.mid(endQuote + 1).trimmed();
            }
        } else {
            // 无引号，第一个空格前是路径
            int spaceIdx = stored.indexOf(' ');
            if (spaceIdx > 0) {
                storedPath = stored.left(spaceIdx);
                storedArgs = stored.mid(spaceIdx + 1).trimmed();
            } else {
                storedPath = stored;
            }
        }

        QString current = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

        if (QDir::toNativeSeparators(storedPath) != current) {
            autostart = 0;
            qCDebug(CountdownLog) << "注册表路径不一致：" << storedPath << " vs " << current;
        } else if (storedArgs.contains("--start-in-tray")) {
            autostart = 2;
        } else if (storedArgs.isEmpty()) {
            autostart = 1;
        } else {
            autostart = 0;
            qCDebug(CountdownLog) << "未知参数：" << storedArgs;
        }
    }
    #elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QString desktopPath = autostartPath + "/" + "com.countdown.desktop";
    QFile file(desktopPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray content = file.readAll();
        QString execLine = "Exec=\"" + QCoreApplication::applicationFilePath() + "\"";

        if(content.contains((execLine + " --start-in-tray").toUtf8())) autostart = 2; // 最小化到托盘启动
        else if(content.contains(execLine.toUtf8())) autostart = 1; // 无参直接启动
        else autostart = 0;

        if(autostart == 0) qCDebug(CountdownLog) << "Desktop 启动路径不一致或参数未知";
    }
    else qCDebug(CountdownLog) << "打开 Desktop 文件失败";

    #endif
    qCDebug(CountdownLog) << "当前" << os << "开机自启为：" << autostart;
    return autostart;
}

// 设定自启
void CountdownAutoStart::setAutoStart(qint64 enable) {
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

    if (enable == 0) {
        settings.remove(appName);
        qCDebug(CountdownLog) << "已移除注册表项";
    } else {
        QString path = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        QString value = "\"" + path + "\"";
        if (enable == 2) {
            value += " --start-in-tray";
        }
        settings.setValue(appName, value);
        qCDebug(CountdownLog) << "已写入注册表：" << value;
    }
    settings.sync();  // 立即落盘，避免延迟写入
    #elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Linux
    QString autostartPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    QString desktopPath = autostartPath + "/" + "com.countdown.desktop";

    if(enable == 0) {
        qCDebug(CountdownLog) << "禁用开机自启";
        if(!QFile::remove(desktopPath)) qCritical() << "禁用自启失败：删除 Desktop 文件失败：" << desktopPath;; // 关自启
        return;
    }

    QFile src(":/qt/qml/com/countdown/com.countdown.desktop");
    if (!src.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法读取内置 desktop 模板";
        return;
    }
    QString content = QString::fromUtf8(src.readAll());
    src.close();

    // 替换占位符
    QString execLine = "Exec=\"" + QCoreApplication::applicationFilePath() + "\"";
    if(enable == 1) content.replace("Exec=Countdown", execLine); // 无参直接启动
    else if(enable == 2) content.replace("Exec=Countdown", execLine + " --start-in-tray"); // 最小化到托盘启动

    // 写到目标位置
    QDir().mkpath(autostartPath);
    QFile dst(desktopPath);

    // 开自启
    if (dst.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&dst);
        out << content;
        qCDebug(CountdownLog) << "启用开机自启";
    }
    else qCritical() << "Desktop 文件写入失败";
    #endif
}
