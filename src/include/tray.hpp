#pragma once
#include <QString>
#include <QJSEngine>
#include <QQmlEngine>
#include <QSystemTrayIcon>

class CountdownTray : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownTray(QObject *parent);

    // 让 QML 单例复用 C++ 的同一个实例
    static CountdownTray *create(QQmlEngine *, QJSEngine *);

    void trayInit();
    void updateTrayIcon();
    void shutdown();
    void showMainWindow();

private:
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
CountdownTray &tray();
