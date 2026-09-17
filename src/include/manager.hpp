#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QJSEngine>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>
#include <QTimer>

class CountdownManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例
    Q_PROPERTY(QVariantList countdowns READ countdowns NOTIFY refreshCountdowns)

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownManager(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownManager *create(QQmlEngine *, QJSEngine *);

    QVariantList countdowns() const;

    Q_INVOKABLE int setting(const QString &key, int def = 0) const;
    Q_INVOKABLE void setSetting(const QString &key, int value);
    void push_reminder();
    Q_INVOKABLE void editCountdown(const QString &dateString);
    Q_INVOKABLE void removeCountdown(int id);
    void run_reminder(int id);


signals:
    void refreshCountdowns();

private:
    void saveCountdowns();
    void loadCountdowns();
    void updateOlddata();
    QJsonObject getCountdownJson(int id, QString key) const;

    QJsonArray m_countdowns;
    QString m_filePath;
    QTimer m_reminderTimer;
};
CountdownManager &manager();
