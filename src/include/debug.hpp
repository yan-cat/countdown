#pragma once
#include <QLoggingCategory>
#include <QString>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

Q_DECLARE_LOGGING_CATEGORY(CountdownLog)

class CountdownDebug : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownDebug(QObject *parent);

    Q_INVOKABLE qint64 getDebugOn(const QString &key);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownDebug *create(QQmlEngine *, QJSEngine *);
};
CountdownDebug &debug();
