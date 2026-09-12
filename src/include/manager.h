#ifndef MANAGER_H
#define MANAGER_H

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
    QVariantList countdowns() const;

    Q_INVOKABLE int setting(const QString &key, int def = 0) const;
    Q_INVOKABLE void setSetting(const QString &key, int value);
    void push_reminder();

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownManager *create(QQmlEngine *, QJSEngine *);

signals:
    void refreshCountdowns();

public slots:
    void editCountdown(const QString &dateString);
    void removeCountdown(int id);
    void run_reminder(int id);

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
#endif