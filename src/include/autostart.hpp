#pragma once

#include <QQmlEngine>

class CountdownAutoStart : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownAutoStart(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownAutoStart *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE qint64 getAutoStart();
    Q_INVOKABLE void setAutoStart(qint64 enable);
};
CountdownAutoStart &autostart();
