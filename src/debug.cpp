// debug.cpp
#include "debug.h"
#include <QSettings>

Q_LOGGING_CATEGORY(CountdownLog, "Countdown.app")

// 初始化函数与统一实例
CountdownDebug *CountdownDebug::create(QQmlEngine *, QJSEngine *) {
    CountdownDebug *m = &debug();                      // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}
CountdownDebug &debug() {
    static CountdownDebug instance;
    return instance;
}

// 获取debug设置
qint64 CountdownDebug::getDebugOn(const QString &key) {
    QSettings settings;
    if (settings.value("debugMode", false).toBool()){
        qint64 s = settings.value(key, false).toInt();
        qCDebug(CountdownLog) << "[ Debug ]" << "获取debug状态值：" << key << "，值：" << s;
        return s;
    }
    else return false;
}