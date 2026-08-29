// debug.cpp
#include "debug.h"
#include <QSettings>


Q_LOGGING_CATEGORY(CountdownLog, "Countdown.app")

// 获取debug设置
qint64 getDebugOn(const QString &key)
{
    QSettings s;
    if (s.value("debugmode", false).toBool()){
        qCDebug(CountdownLog) << "[ Debug ]" << "获取debug状态值：" << key << "，值：" << s.value(key, false).toBool();
        return s.value(key, false).toInt();
    }
    else return false;
}