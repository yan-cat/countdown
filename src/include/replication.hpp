#pragma once
#include <QJSEngine>
#include <QQmlEngine>

class CountdownReplication : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownReplication(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownReplication *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE void exportCountdownData();
    Q_INVOKABLE void importCountdownData();

};
CountdownReplication &replication();
