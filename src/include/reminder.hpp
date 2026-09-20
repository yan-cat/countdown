#pragma once
#include <QString>
#include <QJSEngine>
#include <QQmlEngine>

class CountdownReminder : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownReminder(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownReminder *create(QQmlEngine *, QJSEngine *);

    Q_INVOKABLE void pushReminder(QString title, QString body);

};
CountdownReminder &reminder();
