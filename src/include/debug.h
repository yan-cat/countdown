#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(CountdownLog)

#ifndef DEBUG_H
#define DEBUG_H

#include <QString>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

class CountdownDebug : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    Q_INVOKABLE qint64 getDebugOn(const QString &key);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownDebug *create(QQmlEngine *, QJSEngine *);
};
CountdownDebug &debug();
#endif