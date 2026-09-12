#ifndef UPDATER_H
#define UPDATER_H

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QQmlEngine>            // ← 新增
#include <QJSEngine>
#include <QtQml/qqmlregistration.h>

class CountdownUpdater : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    // QObject *parent = nullptr会直接new一个新实例然后爆炸
    explicit CountdownUpdater(QObject *parent);
    Q_INVOKABLE void getReleaseInfo();
    Q_INVOKABLE void downloadNewVersion();

    // ★ 让 QML 单例复用 C++ 的同一个实例
    static CountdownUpdater *create(QQmlEngine *, QJSEngine *);

private:
    QNetworkAccessManager m_updater;
    QNetworkAccessManager networkManager;
    QFile downloadFile;
    QNetworkReply *reply = nullptr;

signals:
    void newVersion(bool latestVersion,QString version ,QString updateLog);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadError(const QString &errorString);
    void installSuccess();

private:
    void installNewVersion(QString path);
};
CountdownUpdater &updater();
#endif // UPDATER_H
