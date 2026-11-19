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

    // 绑定重算
    Q_PROPERTY(QVariantList countdowns READ countdowns NOTIFY refreshCountdowns)
    Q_PROPERTY(int sortModes READ sortModes NOTIFY refreshCountdowns)


public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownManager(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownManager *create(QQmlEngine *, QJSEngine *);

    QVariantList countdowns() const;

    Q_INVOKABLE int setting(const QString &key, int def = 0) const;
    Q_INVOKABLE void setSetting(const QString &key, int value);
    Q_INVOKABLE bool hasSetting(const QString &key);
    // 设置绑定
    int sortModes() const { return setting("sortModes", 0); }

    void push_reminder();
    Q_INVOKABLE void editCountdown(const QString &dateString);
    Q_INVOKABLE void removeCountdown(int id);
    Q_INVOKABLE void moveCountdown(int from, int to);
    void run_reminder(int id);

    Q_INVOKABLE QString getGuideMarkdown();

    QString getCountdownsPath() const { return m_filePath; } // 获取数据路径

    bool reloadCountdownData();

signals:
    void refreshCountdowns();

private:
    void taskTimer();
    bool loadCountdowns();
    void saveCountdowns();
    void updateOlddata();
    QJsonObject getCountdownJson(int id, QString key) const;

    QJsonArray m_countdowns;
    QString m_filePath;
    QTimer m_taskTimer;
};
CountdownManager &manager();
